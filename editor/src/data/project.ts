import type { CustomJSON, Serializable } from "./serialization";

import { Action } from "./action";
import { ActionSet } from "./actionSet";
import { Manager } from "./manager";
import { Parameter } from "./parameter";
import { Sequence } from "./sequence";
import { Timestamp } from "./timestamp";

// project object project object project object project object
export class Project implements Serializable {
  // TODO: name, description, etc.
  actionSet: ActionSet;
  timestampManager: Manager<Timestamp>;
  sequenceManager: Manager<Sequence>;
  openedSequence?: Sequence;
  // TODO: add way to store order of sequence tabs

  constructor(
    actionSet: ActionSet,
    timestampManager: Manager<Timestamp> = new Manager<Timestamp>(),
    sequenceManager: Manager<Sequence> = new Manager<Sequence>(),
    openedSequence?: Sequence
  ) {
    this.actionSet = actionSet;
    this.timestampManager = timestampManager;
    this.sequenceManager = sequenceManager;
    this.openedSequence = openedSequence;
  }

  toJSON(): CustomJSON<Project> {
    return {
      actionSet: this.actionSet,
      timestampManager: this.timestampManager,
      sequenceManager: this.sequenceManager,
      openedSequence: this.openedSequence?.id,
    };
  }

  static fromJSON(json: ReturnType<typeof this.prototype.toJSON>): Project {
    const managers = {
      actionManager: new Manager<Action>(),
      parameterManager: new Manager<Parameter<any>>(),
      sequenceManager: new Manager<Sequence>(),
      timestampManager: new Manager<Timestamp>(),
    };
    return new Project(
      ActionSet.fromJSON(json.actionSet, managers),
      Manager.fromJSON(
        json.timestampManager,
        managers,
        Timestamp.fromJSON,
        managers.timestampManager
      ),
      Manager.fromJSON(
        json.sequenceManager,
        managers,
        Sequence.fromJSON,
        managers.sequenceManager
      ),
      managers.sequenceManager.children.get(json.openedSequence)
    );
  }
}
