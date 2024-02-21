// Manages a list of a certain type of object, keeping track of their IDs
// In the future this will also initiate serialization and deserialization
export class Manager<T extends Manageable<any>> {
  private idCounter: number = 0;
  children: Map<string, T> = new Map<string, T>();

  add(child: T) {
    if (this.children.has(child.id)) {
      throw new Error(
        `New child ID (${child.id}) already exists in manager object.`
      );
    }
    this.children.set(child.id, child);
  }

  generateId(): string {
    while (String(this.idCounter) in this.children) {
      this.idCounter++;
    }
    return String(this.idCounter++);
  }
}

// A hacky way to define "manageable" objects
export abstract class Manageable<T extends Manageable<T>> {
  readonly id: string;
  readonly manager: Manager<T>;
  private _name: string;
  description: string;

  constructor(
    manager: Manager<T>,
    name: string,
    description: string = "",
    id?: string
  ) {
    this.manager = manager;
    this.name = name;
    this._name = name;
    this.description = description;
    if (id === undefined) {
      this.id = this.manager.generateId();
    } else {
      this.id = id;
    }
    this.add();
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

export type idType = Manageable<any>["id"];
