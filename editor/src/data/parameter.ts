// For right now, a new instance will be created for each parameter of each action
// instance. I may want to separate the Parameter class from the state itself in
// case the Parameter properties are changed during runtime. (TODO)
export abstract class Parameter {
  id: number;
  name: string;
  abstract value?: number | string | boolean;

  constructor(id: number, name: string) {
    this.id = id;
    this.name = name;
  }

  // Sub-classes can have optional restrictions on what constitutes a valid value
  abstract validate(
    value: number | string | boolean
  ): [error: ParameterError, fixed?: number | string | boolean];
}

// May seem redundant but I think it will be useful for serialization purposes
export enum ParameterType {
  Number,
  String,
  Boolean,
  NumberEnum,
  StringEnum,
}

export enum ParameterError {
  // Parameter is valid
  None,
  UnderMin,
  OverMax,
  // Parameter does not conform to the correct step size
  // TODO: potential precision issues?
  WrongStep,
  UnderMinLength,
  OverMaxLength,
}

export class NumberParameter extends Parameter {
  type: ParameterType = ParameterType.Number;
  min?: number;
  max?: number;
  step?: number;
  value?: number;

  constructor(
    id: number,
    name: string,
    min?: number,
    max?: number,
    step?: number
  ) {
    super(id, name);
    this.min = min;
    this.max = max;
    this.step = step;
  }

  validate(value: number): [error: ParameterError, fixed: number] {
    if (this.min !== undefined && value < this.min) {
      return [ParameterError.UnderMin, this.min];
    }
    if (this.max !== undefined && value > this.max) {
      return [ParameterError.OverMax, this.max];
    }
    if (
      this.step !== undefined &&
      Math.round(value / this.step) == value / this.step
    ) {
      let fixed = Math.round(value / this.step) * this.step;
      return [ParameterError.WrongStep, this.validate(fixed)[1]];
    }
    return [ParameterError.None, value];
  }
}

export class StringParameter extends Parameter {
  type: ParameterType = ParameterType.String;
  minLength?: number;
  maxLength?: number;
  value?: number;

  constructor(
    id: number,
    name: string,
    minLength?: number,
    maxLength?: number
  ) {
    super(id, name);
    this.minLength = minLength;
    this.maxLength = maxLength;
  }

  validate(value: string): [error: ParameterError, fixed?: string] {
    if (this.minLength !== undefined && value.length < this.minLength) {
      return [ParameterError.UnderMinLength];
    }
    if (this.maxLength !== undefined && value.length > this.maxLength) {
      return [ParameterError.OverMaxLength, value.slice(0, this.maxLength)];
    }
    return [ParameterError.None, value];
  }
}

export class BooleanParameter extends Parameter {
  type: ParameterType = ParameterType.Boolean;
  value?: number;

  constructor(id: number, name: string) {
    super(id, name);
  }

  validate(value: string): [error: ParameterError] {
    return [ParameterError.None];
  }
}
