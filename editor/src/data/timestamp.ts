import { Manageable, Manager, type IdType } from "./manager";

export class Timestamp extends Manageable<Timestamp> {
  value: number;
  relativeTo?: Timestamp;

  constructor(
    manager: Manager<Timestamp>,
    value: number,
    relativeTo?: Timestamp,
    // TODO: fix it so that name can be undefined?
    name: string = "",
    description?: string,
    id?: IdType,
    hue?: number
  ) {
    super(manager, name, description, id, hue);
    this.value = value;
    this.relativeTo = relativeTo;
  }

  toJSON(): CustomJSON<Timestamp> {
    return {
      ...this.manageableJSON(),
      value: this.value,
      relativeTo: this.relativeTo?.id,
    };
  }

  add() {
    this.manager.add(this);
  }

  getOffset(): [offset: number, root: Timestamp] {
    if (this.relativeTo === undefined) {
      return [0, this];
    }
    const [offset, root] = this.relativeTo.getOffset();
    return [offset + this.value, root];
  }
}
