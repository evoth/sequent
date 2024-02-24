import {
  NestedParameter,
  NumberParameter,
  StringParameter,
} from "./data/parameter";

import { writable } from "svelte/store";
import { Action } from "./data/action";
import { ActionSet } from "./data/actionSet";
import { Project } from "./data/project";

const actionSet = new ActionSet("Test");
const param0 = new NumberParameter<number>(
  actionSet.parameterManager,
  "Test param",
  "",
  7,
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
export const project = writable(new Project(actionSet));
