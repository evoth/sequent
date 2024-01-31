import { Parameter, ParameterState } from "./parameter";

// Represents the general description of an action/command
export class Action {
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
