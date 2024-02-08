// Manages a list of a certain type of object, keeping track of their IDs
// In the future this will also initiate serialization and deserialization
export class Manager<T extends Manageable<any>> {
  private idCounter: number = 0;
  children: { [id: number]: T } = {};

  add(child: T) {
    if (child.id in this.children) {
      throw new Error("New child ID already exists in manager object.");
    }
    this.idCounter = Math.max(child.id + 1, this.idCounter);
    this.children[child.id] = child;
  }

  generateId(): number {
    return this.idCounter++;
  }
}

// A hacky way to define "manageable" objects
export abstract class Manageable<T extends Manageable<T>> {
  id: number;
  manager: Manager<T>;

  constructor(manager: Manager<T>, id?: number) {
    this.manager = manager;
    if (id === undefined) {
      this.id = this.manager.generateId();
    } else {
      this.id = id;
    }
    this.add();
  }

  abstract add(): void;
}
