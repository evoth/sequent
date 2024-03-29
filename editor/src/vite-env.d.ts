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

import { Entries } from "type-fest";

declare global {
  interface ObjectConstructor {
    entries<T extends object>(o: T): Entries<T>;
    keys<T>(obj: T): Array<keyof T>;
  }
}
