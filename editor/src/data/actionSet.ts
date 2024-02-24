import type { CustomJSON, EntityManagers, Serializable } from "./serialization";

import { Action } from "./action";
import { Manager } from "./manager";
import { Parameter } from "./parameter";

export class ActionSet implements Serializable {
  name: string;
  description: string;
  parameterManager: Manager<Parameter<any>>;
  actionManager: Manager<Action>;

  constructor(
    name: string,
    description: string = "",
    parameterManager: Manager<Parameter<any>> = new Manager<Parameter<any>>(),
    actionManager: Manager<Action> = new Manager<Action>()
  ) {
    this.name = name;
    this.description = description;
    this.parameterManager = parameterManager;
    this.actionManager = actionManager;
  }

  toJSON(): CustomJSON<ActionSet> {
    return {
      name: this.name,
      description: this.description,
      parameterManager: this.parameterManager,
      actionManager: this.actionManager,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): ActionSet {
    return new ActionSet(
      json.name,
      json.description,
      Manager.fromJSON(
        json.parameterManager,
        managers,
        Parameter.fromJSON,
        managers.parameterManager
      ),
      Manager.fromJSON(
        json.actionManager,
        managers,
        Action.fromJSON,
        managers.actionManager
      )
    );
  }
}
