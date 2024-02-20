import { Manageable, Manager, type idType } from "./manager";

// Represents the general description of a given parameter (T cannot be nullable)
export class Parameter<T extends {}> extends Manageable<Parameter<any>> {
  readonly defaultValue: T;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T
  ) {
    super(manager, name, description);
    this.defaultValue = defaultValue;
  }

  add() {
    this.manager.add(this);
  }

  // Sub-classes can have optional restrictions on what constitutes a valid value.
  // The `fixed` return value optionally returns a "fixed" version of the value
  // that will pass validation.
  validate(value: T): ParameterValidation<T> {
    return {error: ParameterError.None};
  }

  checkDefault(): void {
    if (this.validate(this.defaultValue).error != ParameterError.None) {
      throw new Error(
        `Default value of ${this.defaultValue} is not valid for parameter with ID ${this.id}.`
      );
    }
  }

  newState(value?: T): ParameterState<Parameter<T>, T> {
    return new ParameterState<Parameter<T>, T>(this, value);
  }
}

// Represents the actual parameter value in an action instance
export class ParameterState<T extends Parameter<U>, U extends {}> {
  readonly parameter: T;
  value: U;

  constructor(parameter: T, value?: U) {
    this.parameter = parameter;
    this.value = value ?? this.parameter.defaultValue;
  }
}

export type ParameterValidation<T> = {
  error: ParameterError;
  fixed?: T,
}

export enum ParameterError {
  None,
  UnderMin,
  OverMax,
  WrongStep,
  UnderMinLength,
  OverMaxLength,
  BadEnumOption,
  BadNestedOption,
  EmptyValue,
}

// Provides number-specific validation
export class NumberParameter<T extends number> extends Parameter<T> {
  readonly min?: T;
  readonly max?: T;
  readonly step?: T;
  readonly unit?: string;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    min?: T,
    max?: T,
    step?: T,
    unit?: string
  ) {
    super(manager, name, description, defaultValue);
    this.min = min;
    this.max = max;
    this.step = step;
    this.unit = unit;
    this.checkDefault();
  }

  validate(value: T): ParameterValidation<T> {
    const epsilon = 1e-10;
    if (this.min !== undefined && value < this.min) {
      return {error: ParameterError.UnderMin, fixed: this.min};
    }
    if (this.max !== undefined && value > this.max) {
      return {error: ParameterError.OverMax, fixed: this.max};
    }
    if (
      this.step !== undefined &&
      Math.abs(Math.round(value / this.step) - value / this.step) > epsilon
    ) {
      let fixed = (Math.round(value / this.step) * this.step) as T;
      fixed = this.validate(fixed).fixed ?? fixed;
      if (fixed !== value) {
        return {error: ParameterError.WrongStep, fixed};
      }
    }
    return {error: ParameterError.None};
  }
}

// Provides string-specific validation
export class StringParameter<T extends string> extends Parameter<T> {
  readonly minLength?: number;
  readonly maxLength?: number;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    minLength?: number,
    maxLength?: number
  ) {
    super(manager, name, description, defaultValue);
    this.minLength = minLength;
    this.maxLength = maxLength;
    this.checkDefault();
  }

  validate(value: T): ParameterValidation<T> {
    if (this.minLength !== undefined && value.length < this.minLength) {
      return {error: ParameterError.UnderMinLength};
    }
    if (this.maxLength !== undefined && value.length > this.maxLength) {
      return {
        error: ParameterError.OverMaxLength,
        fixed: value.slice(0, this.maxLength) as T,
      };
    }
    return {error: ParameterError.None};
  }
}

// Provides validation for enumerations (sets of allowed values)
export class EnumParameter<T extends {}> extends Parameter<T> {
  readonly options: T[];

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    options: T[]
  ) {
    super(manager, name, description, defaultValue);
    if (options.length === 0) {
      throw new Error(
        `EnumParameter with ID ${this.id} has empty options array.`
      );
    }
    this.options = options;
    this.checkDefault();
  }

  validate(value: T): ParameterValidation<T> {
    if (!this.options.includes(value)) {
      return {error: ParameterError.BadEnumOption};
    }
    return {error: ParameterError.None};
  }
}

// Provides validation for nested parameters (values that map to sets of sub-parameters)
export class NestedParameter<T extends {}> extends Parameter<T> {
  readonly nested: Map<T, Parameter<any>[]>;
  readonly descendants: Set<Parameter<any>>;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    nested: Map<T, Parameter<any>[]>
  ) {
    super(manager, name, description, defaultValue);
    if (nested.size === 0) {
      throw new Error(
        `NestedParameter with ID ${this.id} has empty nested parameter map.`
      );
    }
    this.nested = nested;
    this.descendants = this.getDescendants([]);
    this.checkDefault();
  }

  // Gets a set of all nested parameters, checking for self-containing parameters
  private getDescendants(
    ancestors: NestedParameter<any>[]
  ): Set<Parameter<any>> {
    if (ancestors.includes(this)) {
      throw new Error(`NestedParameter with ID ${this.id} contains itself.`);
    }
    const descendants = new Set<Parameter<any>>();
    descendants.add(this);
    for (const option of this.nested.values()) {
      for (const child of option) {
        descendants.add(child);
        if (child instanceof NestedParameter) {
          for (const nestedChild of child.getDescendants([
            ...ancestors,
            this,
          ])) {
            descendants.add(nestedChild);
          }
        }
      }
    }
    return descendants;
  }

  validate(value: T): ParameterValidation<T> {
    if (!this.nested.has(value)) {
      return {error: ParameterError.BadNestedOption};
    }
    return {error: ParameterError.None};
  }
}
