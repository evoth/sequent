import type { Action } from "./action";
import type { Manager } from "./manager";
import type { Parameter } from "./parameter";
import type { Sequence } from "./sequence";
import type { Timestamp } from "./timestamp";

export type CustomJSONValue =
  | any
  | any[]
  | { [key: string]: any }
  | Map<string, any>;

export type CustomJSON<T> = {
  [P in keyof T as T[P] extends Function ? never : P]: CustomJSONValue;
};

export interface Serializable {
  toJSON(): { [key: string]: any };
  // TODO: figure out way to require static fromJSON() method
}

export type EntityManagers = {
  actionManager: Manager<Action>;
  parameterManager: Manager<Parameter<any>>;
  sequenceManager: Manager<Sequence>;
  timestampManager: Manager<Timestamp>;
};

export type FromJSON<T> = (
  json: any,
  managers: EntityManagers,
  genericFromJSON?: FromJSON<any>
) => T;

export type TypeofResult =
  | "string"
  | "number"
  | "bigint"
  | "boolean"
  | "symbol"
  | "undefined"
  | "object"
  | "function";
