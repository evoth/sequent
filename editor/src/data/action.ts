import { Manageable, Manager } from "./manager";
import { NestedParameter, Parameter, ParameterState } from "./parameter";

import type { Repeatable } from "./repeat";
import type { idType } from "./manager";

export type ActionDurationProps = {
  // Duration when durationParam doesn't exist or has no value
  readonly defaultDuration: number;
  // If durationParam exists and has a value, its value is multiplied by
  // durationParamMultiplier, added to durationParamOffset, and used as the
  // duration value
  readonly durationParam?: Parameter<number>;
  readonly durationParamOffset?: number;
  readonly durationParamMultiplier?: number;
};

// Represents the general description of an action/command
export class Action extends Manageable<Action> {
  readonly parameters: Parameter<any>[];
  readonly descendants: Set<Parameter<any>>;
  readonly durationProps: ActionDurationProps;

  constructor(
    manager: Manager<Action>,
    name: string,
    description: string,
    parameters: Parameter<any>[] = [],
    durationProps: ActionDurationProps
  ) {
    super(manager, name, description);
    this.parameters = parameters;
    this.descendants = this.getDescendants();
    if (
      durationProps.durationParam !== undefined &&
      !this.descendants.has(durationProps.durationParam)
    ) {
      throw new Error(
        `Parameter with ID ${durationProps.durationParam.id} cannot be used as durationParam ` +
          `for Action with ID ${this.id} because it is not a descendant parameter.`
      );
    }
    this.durationProps = durationProps;
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
}

// Represents an actual action instance with its parameter values
export class ActionState implements Repeatable {
  readonly action: Action;
  readonly parameterStates: Map<idType, ParameterState<Parameter<any>, any>>;

  constructor(
    action: Action,
    parameterStates: ParameterState<Parameter<any>, any>[]
  ) {
    this.action = action;
    this.parameterStates = new Map<
      idType,
      ParameterState<Parameter<any>, any>
    >();
    for (const paramState of parameterStates) {
      this.parameterStates.set(paramState.parameter.id, paramState);
    }
  }

  // Represents duration of action; see ActionDurationProps type
  getDuration(): number {
    const durationProps = this.action.durationProps;
    if (durationProps.durationParam === undefined)
      return durationProps.defaultDuration;

    const durationParamValue = this.parameterStates.get(
      durationProps.durationParam.id
    )?.value;
    if (durationParamValue === undefined) return durationProps.defaultDuration;

    return (
      (durationProps.durationParamOffset ?? 0) +
      durationParamValue * (durationProps.durationParamMultiplier ?? 1)
    );
  }
}
