// Represents the general description of a given parameter
export class Parameter<T> {
  id: number;
  name: string;
  description: string;

  constructor(id: number, name: string, description: string) {
    this.id = id;
    this.name = name;
    this.description = description;
  }

  // Sub-classes can have optional restrictions on what constitutes a valid value.
  // The `fixed` return value optionally returns a "fixed" version of the value
  // that will pass validation.
  validate(value: T): [error: ParameterError, fixed?: T] {
    return [ParameterError.None];
  }

  newState(value?: T): ParameterState<Parameter<T>, T> {
    return new ParameterState<Parameter<T>, T>(this, value);
  }
}

// Represents the actual parameter value in an action instance
// (I don't think generics are supposed to be used like this...)
export class ParameterState<T extends Parameter<U>, U> {
  parameter: T;
  value?: U;

  constructor(parameter: T, value?: U) {
    this.parameter = parameter;
    this.value = value;
  }
}

export enum ParameterError {
  // Parameter is valid
  None,
  UnderMin,
  OverMax,
  // Parameter does not conform to the correct step size
  WrongStep,
  UnderMinLength,
  OverMaxLength,
  // Value isn't an enum option
  BadEnumOption,
}

// Provides number-specific validation
export class NumberParameter<T extends number> extends Parameter<T> {
  min?: T;
  max?: T;
  step?: T;
  unit?: string;

  constructor(
    id: number,
    name: string,
    description: string,
    min?: T,
    max?: T,
    step?: T,
    unit?: string
  ) {
    super(id, name, description);
    this.min = min;
    this.max = max;
    this.step = step;
    this.unit = unit;
  }

  validate(value: T): [error: ParameterError, fixed?: T] {
    const epsilon = 1e-10;
    if (this.min !== undefined && value < this.min) {
      return [ParameterError.UnderMin, this.min];
    }
    if (this.max !== undefined && value > this.max) {
      return [ParameterError.OverMax, this.max];
    }
    if (
      this.step !== undefined &&
      Math.abs(Math.round(value / this.step) - value / this.step) > epsilon
    ) {
      let fixed = (Math.round(value / this.step) * this.step) as T;
      return [ParameterError.WrongStep, this.validate(fixed)[1] ?? fixed];
    }
    return [ParameterError.None];
  }
}

// Provides string-specific validation
export class StringParameter<T extends string> extends Parameter<T> {
  minLength?: number;
  maxLength?: number;

  constructor(
    id: number,
    name: string,
    description: string,
    minLength?: number,
    maxLength?: number
  ) {
    super(id, name, description);
    this.minLength = minLength;
    this.maxLength = maxLength;
  }

  validate(value: T): [error: ParameterError, fixed?: T] {
    if (this.minLength !== undefined && value.length < this.minLength) {
      return [ParameterError.UnderMinLength];
    }
    if (this.maxLength !== undefined && value.length > this.maxLength) {
      return [
        ParameterError.OverMaxLength,
        value.slice(0, this.maxLength) as T,
      ];
    }
    return [ParameterError.None];
  }
}

// Provides validation for enumerations (sets of allowed values)
export class EnumParameter<T> extends Parameter<T> {
  options: Set<T>;

  constructor(id: number, name: string, description: string, options: T[]) {
    super(id, name, description);
    this.options = new Set<T>(options);
  }

  validate(value: T): [error: ParameterError] {
    if (!this.options.has(value)) {
      return [ParameterError.BadEnumOption];
    }
    return [ParameterError.None];
  }
}
