import { readable, writable } from "svelte/store";
import { Component, Sequence } from "./sequence";

import { getEsp32ActionSet } from "../actionSets/esp32";
import { DB } from "./db";
import { Project } from "./project";
import { toFixedJSON } from "./serialization";

// TODO: Delete test data and figure out what happens for new user

export const project = writable(
  new Project("New project", "", getEsp32ActionSet())
);
export const fileHandle = writable<FileSystemFileHandle | undefined>();

const newDB = new DB("sequent");
newDB.initAutosave(
  project,
  "project",
  (value) => toFixedJSON(value),
  (value) => Project.fromJSON(value)
);
newDB.initAutosave(fileHandle, "fileHandle");
export const db = readable(newDB);

export const updateIndex = writable(0);

export const selectedComponents = writable<
  Map<Sequence, Component | undefined>
>(new Map());
