import { Manageable, Manager } from "./manager";

// Represents the general description of a given parameter
export class Parameter<T> extends Manageable<Parameter<any>> {
  name: string;
  description: string;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string
  ) {
    super(manager);
    this.name = name;
    this.description = description;
  }

  add() {
    this.manager.add(this);
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
  // Value isn't an option that exists
  BadEnumOption,
  BadNestedOption,
}

// Provides number-specific validation
export class NumberParameter<T extends number> extends Parameter<T> {
  min?: T;
  max?: T;
  step?: T;
  unit?: string;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    min?: T,
    max?: T,
    step?: T,
    unit?: string
  ) {
    super(manager, name, description);
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
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    minLength?: number,
    maxLength?: number
  ) {
    super(manager, name, description);
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
  options: T[];

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    options: T[]
  ) {
    super(manager, name, description);
    this.options = options;
  }

  validate(value: T): [error: ParameterError] {
    if (!this.options.includes(value)) {
      return [ParameterError.BadEnumOption];
    }
    return [ParameterError.None];
  }
}

// Provides validation for nested parameters (values that map to sets of sub-parameters)
export class NestedParameter<T> extends Parameter<T> {
  nested: Map<T, Parameter<any>[]>;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    nested: Map<T, Parameter<any>[]>
  ) {
    super(manager, name, description);
    this.nested = nested;
  }

  // Gets a set of all nested parameters, checking for self-containing parameters
  getChildren(ancestors: NestedParameter<any>[]): Set<Parameter<any>> {
    if (ancestors.includes(this)) {
      throw new Error(`NestedParameter with ID ${this.id} contains itself.`);
    }
    const children = new Set<Parameter<any>>();
    children.add(this);
    for (const option of this.nested.values()) {
      for (const child of option) {
        children.add(child);
        if (child instanceof NestedParameter) {
          for (const nestedChild of child.getChildren([...ancestors, this])) {
            children.add(nestedChild);
          }
        }
      }
    }
    return children;
  }

  validate(value: T): [error: ParameterError] {
    if (!this.nested.has(value)) {
      return [ParameterError.BadNestedOption];
    }
    return [ParameterError.None];
  }
}
