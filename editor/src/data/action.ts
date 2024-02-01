import { Parameter, ParameterState } from "./parameter";

import type { Repeatable } from "./repeat";

// Represents the general description of an action/command
export class Action implements Repeatable {
  id: number;
  name: string;
  description: string;
  parameters: Parameter<any>[];
  requiredParamIds: Set<number>;

  constructor(
    id: number,
    name: string,
    description: string,
    parameters: Parameter<any>[] = [],
    requiredParamIds: number[] = []
  ) {
    this.id = id;
    this.name = name;
    this.description = description;
    this.parameters = parameters;
    this.requiredParamIds = new Set<number>(requiredParamIds);
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

  // TODO: Add a way to represent length of time taken by action
  getDuration(): number | undefined {
    return undefined;
  }
}

// Represents an actual action instance with its parameter values
export class ActionState {
  action: Action;
  parameterStates: ParameterState<Parameter<any>, any>[];

  constructor(
    action: Action,
    parameterStates: ParameterState<Parameter<any>, any>[]
  ) {
    this.action = action;
    this.parameterStates = parameterStates;
  }
}
