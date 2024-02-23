/// <reference types="svelte" />
/// <reference types="vite/client" />
declare type Item = import("svelte-dnd-action").Item;
declare type DndEvent<ItemType = Item> =
  import("svelte-dnd-action").DndEvent<ItemType>;
declare namespace svelteHTML {
  interface HTMLAttributes<T> {
    "on:consider"?: (
      event: CustomEvent<DndEvent<ItemType>> & { target: EventTarget & T }
    ) => void;
    "on:finalize"?: (
      event: CustomEvent<DndEvent<ItemType>> & { target: EventTarget & T }
    ) => void;
  }
}

interface SequenceItem extends Item {
  id: string;
  sequence: Sequence;
}

type CustomJSONTypes =
  | boolean
  | number
  | string
  | undefined
  | null
  | Serializable;
type CustomJSONValue =
  | CustomJSONTypes
  | CustomJSONTypes[]
  | { [key: string]: CustomJSONValue }
  | Map<string, CustomJSONValue>;

type CustomJSON<T> = {
  [P in keyof T as T[P] extends Function ? never : P]: CustomJSONValue;
};

interface Serializable {
  toJSON: { [key: string]: any };
}

type TypeofResult =
  | "string"
  | "number"
  | "bigint"
  | "boolean"
  | "symbol"
  | "undefined"
  | "object"
  | "function";
