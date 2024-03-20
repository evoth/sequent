import type { Action } from "./action";
import type { Manager } from "./manager";
import type { Parameter } from "./parameter";
import type { Sequence } from "./sequence";

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
  // TODO: figure out way to require static fromJSON() method?
}

export type EntityManagers = {
  actionManager: Manager<Action>;
  parameterManager: Manager<Parameter<any>>;
  sequenceManager: Manager<Sequence>;
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

export function toFixedJSON(json: object) {
  return JSON.parse(toJSONString(json));
}

export function toJSONString(json: object) {
  return JSON.stringify(json, (key, value) =>
    value instanceof Map ? Object.fromEntries(value) : value
  );
}

const SERIALIZATION_ROUND_DIGITS = 6;

export function serializeRound(num: number) {
  // https://stackoverflow.com/a/11832950
  return (
    Math.round((num + Number.EPSILON) * 10 ** SERIALIZATION_ROUND_DIGITS) /
    10 ** SERIALIZATION_ROUND_DIGITS
  );
}
