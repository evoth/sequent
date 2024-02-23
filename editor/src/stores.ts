import { Action } from './data/action';
import { ActionSet } from './data/actionSet';
import { Parameter } from './data/parameter';
import { Project } from './data/project';
import { writable } from 'svelte/store';

const actionSet = new ActionSet("Test");
new Action(actionSet.actionManager, "Photo", "Trigger shutter with given settings", {defaultDuration: 1});
export const project = writable(new Project(actionSet));