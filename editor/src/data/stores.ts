import { readable, writable } from "svelte/store";
import {
  EnumParameter,
  NestedParameter,
  NumberParameter,
  Parameter,
  StringParameter,
} from "./parameter";
import { Component, Sequence } from "./sequence";

import { Action } from "./action";
import { ActionSet } from "./actionSet";
import { DB } from "./db";
import { Project } from "./project";
import { toFixedJSON } from "./serialization";

// TODO: Delete test data and figure out what happens for new user
const actionSet = new ActionSet("Test");
const param0 = new NumberParameter<number>(
  actionSet.parameterManager,
  "Test param 0",
  "",
  7,
  -13,
  undefined,
  undefined,
  "seconds",
  "3"
);
const param1 = new StringParameter<string>(
  actionSet.parameterManager,
  "Test param 1 ",
  "",
  "defgecd",
  1,
  7,
  "2"
);
const param2 = new NestedParameter<string>(
  actionSet.parameterManager,
  "Test param 2",
  "",
  "beanz",
  new Map(Object.entries({ beanz: [param0], tim: [param1, param0], ".": [] })),
  "1"
);
const param3 = new NestedParameter<string>(
  actionSet.parameterManager,
  "Test param 3",
  "",
  "beanz",
  new Map(Object.entries({ beanz: [param2], tim: [param0], ".": [] })),
  "0"
);
const param4 = new Parameter<boolean>(
  actionSet.parameterManager,
  "Tim beeg?",
  "",
  true
);
const param5 = new EnumParameter(
  actionSet.parameterManager,
  "Exist?",
  "",
  "maybe",
  ["yes", "maybe"]
);
new Action(
  actionSet.actionManager,
  "Photo",
  "Trigger shutter with given settings",
  {
    defaultDuration: 1,
    durationParam: param0,
  },
  [param0, param2, param1, param3, param4, param5]
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

export const updateIndex = writable(0);

export const selectedComponents = writable<
  Map<Sequence, Component | undefined>
>(new Map());
