import { Manageable, Manager } from "./manager";
import { Parameter, ParameterState } from "./parameter";

import type { Repeatable } from "./repeat";

// Represents the general description of an action/command
export class Action extends Manageable<Action> {
  name: string;
  description: string;
  parameters: Parameter<any>[];
  requiredParamIds: Set<number>;

  constructor(
    manager: Manager<Action>,
    name: string,
    description: string,
    parameters: Parameter<any>[] = [],
    requiredParamIds: number[] = []
  ) {
    super(manager);
    this.name = name;
    this.description = description;
    this.parameters = parameters;
    this.requiredParamIds = new Set<number>(requiredParamIds);
  }

  add() {
    this.manager.add(this);
  }

  isParameterRequired(parameter: Parameter<any>): boolean {
    return this.requiredParamIds.has(parameter.id);
  }

  newState(): ActionState {
    const states = this.parameters.map((parameter: Parameter<any>) =>
      parameter.newState()
    );
    return new ActionState(this, states);
  }
}

// Represents an actual action instance with its parameter values
export class ActionState implements Repeatable {
  action: Action;
  parameterStates: ParameterState<Parameter<any>, any>[];

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
