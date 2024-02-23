import { Action } from "./action";
import { Manager } from "./manager";
import { Parameter } from "./parameter";

export class ActionSet implements Serializable {
  name: string;
  description: string;
  actionManager: Manager<Action>;
  parameterManager: Manager<Parameter<any>>;

  constructor(
    name: string,
    description: string = "",
    actionManager: Manager<Action> = new Manager<Action>(),
    parameterManager: Manager<Parameter<any>> = new Manager<Parameter<any>>()
  ) {
    this.name = name;
    this.description = description;
    this.actionManager = actionManager;
    this.parameterManager = parameterManager;
  }

  toJSON(): CustomJSON<ActionSet> {
    return {
      name: this.name,
      description: this.description,
      actionManager: this.actionManager,
      parameterManager: this.parameterManager,
    };
  }
}
