import type {
  CustomJSON,
  EntityManagers,
  FromJSON,
  Serializable,
} from "./serialization";

export type IdType = string;

// Manages a list of a certain type of object, keeping track of their IDs
// In the future this will also initiate serialization and deserialization
export class Manager<T extends Manageable<any>> implements Serializable {
  idCounter: number = 0;
  children: Map<IdType, T> = new Map<IdType, T>();

  toJSON(): {
    idCounter: number;
    children: Map<IdType, T>;
  } {
    return { idCounter: this.idCounter, children: this.children };
  }

  // Manager is a special case; its only job is to populate the already created object
  static fromJSON<T extends Manageable<any>>(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers,
    genericFromJSON: FromJSON<T>,
    newManager: Manager<T>
  ): Manager<T> {
    for (const child of Object.values(json.children)) {
      genericFromJSON(child, managers);
    }
    return newManager;
  }

  add(child: T) {
    if (this.children.has(child.id)) {
      throw new Error(
        `New child ID (${child.id}) already exists in manager object.`
      );
    }
    this.children.set(child.id, child);
  }

  generateId(): IdType {
    while (String(this.idCounter) in this.children) {
      this.idCounter++;
    }
    return String(this.idCounter++);
  }
}

// A hacky way to define "manageable" objects
export abstract class Manageable<T extends Manageable<T>>
  implements Serializable
{
  readonly id: IdType;
  readonly manager: Manager<T>;
  private _name: string;
  description: string;
  hue: number;

  constructor(
    manager: Manager<T>,
    name: string,
    description: string = "",
    id?: IdType,
    hue: number = Math.floor(Math.random() * 360)
  ) {
    this.manager = manager;
    this.name = name;
    this._name = name;
    this.description = description;
    this.hue = hue;
    if (id === undefined) {
      this.id = this.manager.generateId();
    } else {
      this.id = id;
    }
    this.add();
  }

  abstract toJSON(): CustomJSON<Manageable<T>>;

  manageableJSON(): CustomJSON<Manageable<any>> {
    return {
      id: this.id,
      manager: null,
      name: this.name,
      description: this.description,
      hue: this.hue,
    };
  }

  set name(newName: string) {
    if (newName === "") {
      throw new Error("Name of manageable object cannot be the empty string.");
    }
    this._name = newName;
  }

  get name() {
    return this._name;
  }

  abstract add(): void;
}
