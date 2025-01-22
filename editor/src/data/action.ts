import { Manageable, Manager } from "./manager";
import { NestedParameter, Parameter, ParameterState } from "./parameter";
import {
  type CustomJSON,
  type EntityManagers,
  type Serializable,
} from "./serialization";

import type { IdType } from "./manager";
import { Render } from "./render";
import type { Repeatable } from "./repeat";

export type ActionDurationParam = {
  // If durationParam exists and has a value, its value is multiplied by
  // durationParamMultiplier, added to durationParamOffset, and used as the
  // duration value
  readonly param: Parameter<number>;
  readonly paramOffset?: number;
  readonly paramMultiplier?: number;
};

export type ActionDurationProps = {
  // Duration when durationParam doesn't exist or has no value
  readonly defaultDuration: number;
  // First param which results in greater than 0 duration is used to calculate duration
  readonly durationParams: ActionDurationParam[];
};

// Represents the general description of an action/command
export class Action extends Manageable<Action> {
  readonly parameters: Parameter<any>[];
  readonly descendants: Set<Parameter<any>>;
  readonly durationProps: ActionDurationProps;
  readonly displayParams: [
    parameter: Parameter<any>,
    name?: string,
    valueTemplate?: string
  ][];

  constructor(
    manager: Manager<Action>,
    name: string,
    description: string,
    durationProps: ActionDurationProps,
    parameters: Parameter<any>[] = [],
    displayParams: [
      parameter: Parameter<any>,
      name?: string,
      valueTemplate?: string
    ][],
    id?: IdType,
    hue?: number
  ) {
    super(manager, name, description, id, hue);
    this.parameters = parameters;
    this.descendants = this.getDescendants();
    for (const durationParam of durationProps.durationParams) {
      if (!this.descendants.has(durationParam.param)) {
        throw new Error(
          `Parameter with ID ${durationParam.param.id} cannot be used as durationParam ` +
            `for Action with ID ${this.id} because it is not a descendant parameter.`
        );
      }
    }
    this.durationProps = durationProps;
    for (const [displayParam, name, valueTemplate] of displayParams) {
      if (!this.descendants.has(displayParam)) {
        throw new Error(
          `Parameter with ID ${displayParam.id} cannot be used as displayParam ` +
            `for Action with ID ${this.id} because it is not a descendant parameter.`
        );
      }
    }
    this.displayParams = displayParams;
  }

  toJSON(): Omit<CustomJSON<Action>, "descendants"> {
    return {
      ...this.manageableJSON(),
      parameters: this.parameters.map((parameter) => parameter.id),
      durationProps: {
        defaultDuration: this.durationProps.defaultDuration,
        durationParams: this.durationProps.durationParams.map(
          (durationParam) => {
            return {
              param: durationParam.param.id,
              paramOffset: durationParam.paramOffset,
              paramMultiplier: durationParam.paramMultiplier,
            };
          }
        ),
      },
      displayParams: this.displayParams.map(
        ([displayParam, name, valueTemplate]) => [
          displayParam.id,
          name,
          valueTemplate,
        ]
      ),
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Action {
    return new Action(
      managers.actionManager,
      json.name,
      json.description,
      {
        defaultDuration: json.durationProps.defaultDuration,
        durationParams: json.durationProps.durationParams.map(
          (paramJson: any) => {
            return {
              param: managers.parameterManager.children.get(paramJson.param),
              paramOffset: paramJson.paramOffset,
              paramMultiplier: paramJson.paramMultiplier,
            };
          }
        ),
      },
      json.parameters.map(
        (id: IdType) => managers.parameterManager.children.get(id),
        json.id,
        json.hue
      ),
      json.displayParams.map(
        ([displayParamId, name, valueTemplate]: [IdType, string, string]) => {
          return [
            managers.parameterManager.children.get(displayParamId),
            name,
            valueTemplate,
          ];
        }
      ),
      json.id,
      json.hue
    );
  }

  add() {
    this.manager.add(this);
  }

  private getDescendants(): Set<Parameter<any>> {
    const descendants = new Set<Parameter<any>>();
    for (const child of this.parameters) {
      descendants.add(child);
      if (child instanceof NestedParameter) {
        for (const nestedChild of child.descendants) {
          descendants.add(nestedChild);
        }
      }
    }
    return descendants;
  }

  newState(): ActionState {
    const allParameters = Array.from(this.descendants);
    const states = allParameters.map((parameter: Parameter<any>) =>
      parameter.newState()
    );
    return new ActionState(this, states);
  }

  getChildIds(): IdType[] {
    return [];
  }
}

// Represents an actual action instance with its parameter values
export class ActionState implements Repeatable, Serializable {
  readonly action: Action;
  readonly parameterStates: Map<IdType, ParameterState<Parameter<any>, any>>;

  constructor(
    action: Action,
    parameterStates: ParameterState<Parameter<any>, any>[]
  ) {
    this.action = action;
    this.parameterStates = new Map<
      IdType,
      ParameterState<Parameter<any>, any>
    >();
    for (const paramState of parameterStates) {
      this.parameterStates.set(paramState.parameter.id, paramState);
    }
  }

  toJSON(): CustomJSON<ActionState> {
    return {
      action: this.action.id,
      parameterStates: [...this.parameterStates.values()],
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): ActionState {
    return new ActionState(
      managers.actionManager.children.get(json.action)!,
      json.parameterStates.map((paramStateJson: any) =>
        ParameterState.fromJSON(paramStateJson, managers)
      )
    );
  }

  // Represents duration of action; see ActionDurationProps type
  getDuration(): number {
    const durationProps = this.action.durationProps;
    for (const durationParam of this.action.durationProps.durationParams) {
      const durationParamValue = this.parameterStates.get(
        durationParam.param.id
      )?.value;

      if (durationParamValue === undefined)
        return durationProps.defaultDuration;

      const duration =
        (durationParam.paramOffset ?? 0) +
        durationParamValue * (durationParam.paramMultiplier ?? 1);
      if (duration > 0) {
        return duration;
      }
    }

    return durationProps.defaultDuration;
  }

  getManageableChild(): Manageable<Action> {
    return this.action;
  }

  getActiveParameters(): Set<Parameter<any>> {
    const active = new Set<Parameter<any>>();

    const bfsQueue = [...this.action.parameters];
    while (bfsQueue.length > 0) {
      const param = bfsQueue.shift();
      if (param === undefined) continue;

      active.add(param);
      if (param instanceof NestedParameter) {
        for (const nestedChild of param.nested.get(
          this.parameterStates.get(param.id)?.value
        ) ?? []) {
          bfsQueue.push(nestedChild);
        }
      }
    }

    return active;
  }

  render(): Render {
    const duration = this.getDuration();
    const render = new Render();
    render.addChild(0, duration, render.baseLayer, {
      action: this.action.id,
      states: Object.fromEntries(
        [...this.parameterStates.entries()].map(([id, state]) => [
          id,
          state.parameter.getRenderValue(state.value),
        ])
      ),
    });
    return render;
  }
}
