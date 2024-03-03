import { readable, writable } from "svelte/store";
import { NestedParameter, NumberParameter, StringParameter } from "./parameter";

import { Action } from "./action";
import { ActionSet } from "./actionSet";
import { DB } from "./db";
import { Project } from "./project";
import { toFixedJSON } from "./serialization";

// TODO: Delete test data and figure out what happens for new user
const actionSet = new ActionSet("Test");
const param0 = new NumberParameter<number>(
  actionSet.parameterManager,
  "Test param",
  "",
  400,
  undefined,
  undefined,
  undefined,
  undefined,
  "3"
);
const param1 = new StringParameter<string>(
  actionSet.parameterManager,
  "Test param",
  "",
  "defgecd",
  undefined,
  undefined,
  "2"
);
const param2 = new NestedParameter<string>(
  actionSet.parameterManager,
  "Test param",
  "",
  "beanz",
  new Map(Object.entries({ beanz: [param0], tim: [param1, param0], ".": [] })),
  "1"
);
const param3 = new NestedParameter<string>(
  actionSet.parameterManager,
  "Test param",
  "",
  "beanz",
  new Map(Object.entries({ beanz: [param2], tim: [param0], ".": [] })),
  "0"
);
new Action(
  actionSet.actionManager,
  "Photo",
  "Trigger shutter with given settings",
  { defaultDuration: 1, durationParam: param0 },
  [param0, param2, param1, param3]
);
export const project = writable(new Project("New project", "", actionSet));
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
