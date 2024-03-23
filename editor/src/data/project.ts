import { type CustomJSON, type Serializable } from "./serialization";

import { getEsp32ActionSet } from "../actionSets/esp32";
import { Action } from "./action";
import { ActionSet } from "./actionSet";
import { Manager } from "./manager";
import { Parameter } from "./parameter";
import { Sequence } from "./sequence";

// project object project object project object project object
export class Project implements Serializable {
  name: string;
  description: string;
  actionSet: ActionSet;
  sequenceManager: Manager<Sequence>;
  openedSequence?: Sequence;
  // TODO: add way to store order of sequence tabs

  constructor(
    name: string,
    description: string = "",
    actionSet: ActionSet,
    sequenceManager: Manager<Sequence> = new Manager<Sequence>(),
    openedSequence?: Sequence
  ) {
    this.name = name;
    this.description = description;
    this.actionSet = actionSet;
    this.sequenceManager = sequenceManager;
    this.openedSequence = openedSequence;
  }

  toJSON(): CustomJSON<Project> & { schemaVersion: number } {
    return {
      name: this.name,
      description: this.description,
      actionSet: this.actionSet,
      sequenceManager: this.sequenceManager,
      openedSequence: this.openedSequence?.id,
      schemaVersion: 0,
    };
  }

  static fromJSON(json: ReturnType<typeof this.prototype.toJSON>): Project {
    const managers = {
      actionManager: new Manager<Action>(),
      parameterManager: new Manager<Parameter<any>>(),
      sequenceManager: new Manager<Sequence>(),
    };
    return new Project(
      json.name,
      json.description,
      // TODO: Add ability to select and upgrade action sets
      //ActionSet.fromJSON(json.actionSet, managers),
      getEsp32ActionSet(managers.parameterManager, managers.actionManager),
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
