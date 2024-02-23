import { Action } from "./data/action";
import { ActionSet } from "./data/actionSet";
import { Parameter } from "./data/parameter";
import { Project } from "./data/project";
import { writable } from "svelte/store";

const actionSet = new ActionSet("Test");
const param1 = new Parameter<number>(
  actionSet.parameterManager,
  "Test param",
  "",
  7
);
new Action(
  actionSet.actionManager,
  "Photo",
  "Trigger shutter with given settings",
  { defaultDuration: 1 },
  [param1]
);
export const project = writable(new Project(actionSet));
