import { Manageable, Manager, type IdType } from "./manager";

import type {
  CustomJSON,
  EntityManagers,
  Serializable,
  TypeofResult,
} from "./serialization";

export type ParameterType = number | string | boolean;

type ParameterJSON<T> = CustomJSON<Manageable<any>> & {
  defaultValue: T;
  type: string;
  valueType: TypeofResult;
};

// Represents the general description of a given parameter (T cannot be nullable)
export class Parameter<T extends ParameterType> extends Manageable<
  Parameter<any>
> {
  readonly defaultValue: T;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    id?: IdType
  ) {
    super(manager, name, description, id);
    this.defaultValue = defaultValue;
  }

  toJSON(): ParameterJSON<T> {
    return this.parameterJson();
  }

  parameterJson(): ParameterJSON<T> {
    return {
      ...this.manageableJSON(),
      type: "Parameter",
      valueType: typeof this.defaultValue,
      defaultValue: this.defaultValue,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Parameter<any> {
    const firstParams = [
      managers.parameterManager,
      json.name,
      json.description,
      json.defaultValue,
    ] as const;
    if (json.type === "Parameter") {
      if (json.valueType === "number") {
        return new Parameter<number>(...firstParams, json.id);
      } else if (json.valueType === "string") {
        return new Parameter<string>(...firstParams, json.id);
      } else if (json.valueType === "boolean") {
        return new Parameter<boolean>(...firstParams, json.id);
      } else {
        throw Error(
          "Unexpected value for 'valueType' when deserializing Parameter."
        );
      }
    } else if (json.type === "NumberParameter") {
      const numJson = json as ReturnType<
        typeof NumberParameter.prototype.toJSON
      >;
      return new NumberParameter<number>(
        ...firstParams,
        numJson.min,
        numJson.max,
        numJson.step,
        numJson.unit,
        json.id
      );
    } else if (json.type === "StringParameter") {
      const stringJson = json as ReturnType<
        typeof StringParameter.prototype.toJSON
      >;
      return new StringParameter<string>(
        ...firstParams,
        stringJson.minLength,
        stringJson.maxLength,
        json.id
      );
    } else if (json.type === "EnumParameter") {
      const enumJson = json as ReturnType<
        typeof EnumParameter.prototype.toJSON
      >;
      if (enumJson.valueType === "number") {
        return new EnumParameter<number>(
          ...firstParams,
          enumJson.options,
          json.id
        );
      } else if (json.valueType === "string") {
        return new EnumParameter<string>(
          ...firstParams,
          enumJson.options,
          json.id
        );
      } else if (json.valueType === "boolean") {
        return new EnumParameter<boolean>(
          ...firstParams,
          enumJson.options,
          json.id
        );
      } else {
        throw Error(
          "Unexpected value for 'valueType' when deserializing EnumParameter."
        );
      }
    } else if (json.type === "NestedParameter") {
      const nestedJson = json as ReturnType<
        typeof NestedParameter.prototype.toJSON
      >;
      const nestedEntries = Object.entries(nestedJson.nested);
      const getMap = (parse: (value: string) => any) =>
        new Map(
          nestedEntries.map(([key, paramIds]) => [
            parse(key),
            paramIds.map(
              (id: IdType) => managers.parameterManager.children.get(id)!
            ),
          ])
        );
      if (nestedJson.valueType === "number") {
        return new NestedParameter<number>(
          ...firstParams,
          getMap(Number),
          json.id
        );
      } else if (json.valueType === "string") {
        return new NestedParameter<string>(
          ...firstParams,
          getMap(String),
          json.id
        );
      } else if (json.valueType === "boolean") {
        return new NestedParameter<boolean>(
          ...firstParams,
          getMap((value: string) => value === "true"),
          json.id
        );
      } else {
        throw Error(
          "Unexpected value for 'valueType' when deserializing NestedParameter."
        );
      }
    } else {
      throw Error("Unexpected value for 'type' when deserializing Parameter.");
    }
  }

  add() {
    this.manager.add(this);
  }

  // Sub-classes can have optional restrictions on what constitutes a valid value.
  // The `fixed` return value optionally returns a "fixed" version of the value
  // that will pass validation.
  validate(value: T): ParameterValidation<T> {
    return { error: ParameterError.None };
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

  getChildIds(): IdType[] {
    return [];
  }
}

// Represents the actual parameter value in an action instance
export class ParameterState<T extends Parameter<U>, U extends ParameterType>
  implements Serializable
{
  readonly parameter: T;
  value: U;

  constructor(parameter: T, value?: U) {
    this.parameter = parameter;
    this.value = value ?? this.parameter.defaultValue;
  }

  toJSON(): { parameter: IdType; value: U } {
    return { parameter: this.parameter.id, value: this.value };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): ParameterState<Parameter<ParameterType>, ParameterType> {
    const parameter = managers.parameterManager.children.get(json.parameter)!;
    type T = typeof parameter;
    type U = typeof parameter.defaultValue;
    return new ParameterState<T, U>(parameter, json.value);
  }
}

export type ParameterValidation<T> = {
  error: ParameterError;
  fixed?: T;
};

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
    unit?: string,
    id?: IdType
  ) {
    super(manager, name, description, defaultValue, id);
    this.min = min;
    this.max = max;
    this.step = step;
    this.unit = unit;
    this.checkDefault();
  }

  toJSON(): ParameterJSON<T> & CustomJSON<NumberParameter<number>> {
    return {
      ...this.parameterJson(),
      type: "NumberParameter",
      min: this.min,
      max: this.max,
      step: this.step,
      unit: this.unit,
    };
  }

  validate(value: T): ParameterValidation<T> {
    const epsilon = 1e-10;
    if (this.min !== undefined && value < this.min) {
      return { error: ParameterError.UnderMin, fixed: this.min };
    }
    if (this.max !== undefined && value > this.max) {
      return { error: ParameterError.OverMax, fixed: this.max };
    }
    if (
      this.step !== undefined &&
      Math.abs(Math.round(value / this.step) - value / this.step) > epsilon
    ) {
      let fixed = (Math.round(value / this.step) * this.step) as T;
      fixed = this.validate(fixed).fixed ?? fixed;
      if (fixed !== value) {
        return { error: ParameterError.WrongStep, fixed };
      }
    }
    return { error: ParameterError.None };
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
    maxLength?: number,
    id?: IdType
  ) {
    super(manager, name, description, defaultValue, id);
    this.minLength = minLength;
    this.maxLength = maxLength;
    this.checkDefault();
  }

  toJSON(): ParameterJSON<T> & CustomJSON<StringParameter<string>> {
    return {
      ...this.parameterJson(),
      type: "StringParameter",
      minLength: this.minLength,
      maxLength: this.maxLength,
    };
  }

  validate(value: T): ParameterValidation<T> {
    if (this.minLength !== undefined && value.length < this.minLength) {
      return { error: ParameterError.UnderMinLength };
    }
    if (this.maxLength !== undefined && value.length > this.maxLength) {
      return {
        error: ParameterError.OverMaxLength,
        fixed: value.slice(0, this.maxLength) as T,
      };
    }
    return { error: ParameterError.None };
  }
}

// Provides validation for enumerations (sets of allowed values)
export class EnumParameter<T extends ParameterType> extends Parameter<T> {
  readonly options: T[];

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    options: T[],
    id?: IdType
  ) {
    super(manager, name, description, defaultValue, id);
    if (options.length === 0) {
      throw new Error(
        `EnumParameter with ID ${this.id} has empty options array.`
      );
    }
    this.options = options;
    this.checkDefault();
  }

  toJSON(): ParameterJSON<T> & { options: T[] } {
    return {
      ...this.parameterJson(),
      type: "EnumParameter",
      options: this.options,
    };
  }

  validate(value: T): ParameterValidation<T> {
    if (!this.options.includes(value)) {
      return { error: ParameterError.BadEnumOption };
    }
    return { error: ParameterError.None };
  }
}

// Provides validation for nested parameters (values that map to sets of sub-parameters)
export class NestedParameter<T extends ParameterType> extends Parameter<T> {
  readonly nested: Map<T, Parameter<any>[]>;
  readonly descendants: Set<Parameter<any>>;

  constructor(
    manager: Manager<Parameter<any>>,
    name: string,
    description: string,
    defaultValue: T,
    nested: Map<T, Parameter<any>[]>,
    id?: IdType
  ) {
    super(manager, name, description, defaultValue, id);
    if (nested.size === 0) {
      throw new Error(
        `NestedParameter with ID ${this.id} has empty nested parameter map.`
      );
    }
    this.nested = nested;
    this.descendants = this.getDescendants([]);
    this.checkDefault();
  }

  toJSON(): ParameterJSON<T> & { nested: Map<string, IdType[]> } {
    const nestedIds = new Map<string, IdType[]>();
    for (const [key, parameters] of this.nested.entries()) {
      nestedIds.set(
        String(key),
        parameters.map((parameter) => parameter.id)
      );
    }
    return {
      ...this.parameterJson(),
      type: "NestedParameter",
      nested: nestedIds,
    };
  }

  // Gets a set of all nested parameters, checking for self-containing parameters
  private getDescendants(
    ancestors: NestedParameter<any>[]
  ): Set<Parameter<any>> {
    if (ancestors.includes(this)) {
      throw new Error(`NestedParameter with ID ${this.id} contains itself.`);
    }
    const descendants = new Set<Parameter<any>>();
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
      return { error: ParameterError.BadNestedOption };
    }
    return { error: ParameterError.None };
  }

  getChildIds(): IdType[] {
    let childIds: IdType[] = [];
    for (const childList of this.nested.values()) {
      childIds = [...childIds, ...childList.map((childParam) => childParam.id)];
    }
    return childIds;
  }
}
