import { Manageable, Manager } from "./manager";
import { NestedParameter, Parameter, ParameterState } from "./parameter";

import type { Repeatable } from "./repeat";

// Represents the general description of an action/command
export class Action extends Manageable<Action> {
  readonly name: string;
  readonly description: string;
  readonly parameters: Parameter<any>[];
  readonly descendants: Set<Parameter<any>>;

  constructor(
    manager: Manager<Action>,
    name: string,
    description: string,
    parameters: Parameter<any>[] = []
  ) {
    super(manager);
    this.name = name;
    this.description = description;
    this.parameters = parameters;
    this.descendants = this.getDescendants();
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
  readonly parameterStates: ParameterState<Parameter<any>, any>[];

  constructor(
    action: Action,
    parameterStates: ParameterState<Parameter<any>, any>[]
  ) {
    this.action = action;
    this.parameterStates = parameterStates;
  }

  // TODO: Add a way to represent length of time taken by action
  getDuration(): number | undefined {
    return undefined;
  }
}
