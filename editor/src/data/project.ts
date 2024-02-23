import { ActionSet } from "./actionSet";
import { Manager } from "./manager";
import { Sequence } from "./sequence";
import { Timestamp } from "./timestamp";

// project object project object project object project object
export class Project {
    // TODO: name, description, etc.
    actionSet: ActionSet;
    sequenceManager: Manager<Sequence>;
    timestampManager: Manager<Timestamp>;
    openedSequence?: Sequence;
    // TODO: add way to store order of sequence tabs

    constructor(actionSet: ActionSet, sequenceManager: Manager<Sequence> = new Manager<Sequence>, timestampManager: Manager<Timestamp> = new Manager<Timestamp>, openedSequence?: Sequence) {
        this.actionSet = actionSet;
        this.sequenceManager = sequenceManager;
        this.timestampManager = timestampManager;
        this.openedSequence = openedSequence;
    }
}