import { Manageable, Manager } from "./manager";

export class Timestamp extends Manageable<Timestamp> {
  value: number;
  relativeTo?: Timestamp;

  constructor(
    manager: Manager<Timestamp>,
    value: number,
    relativeTo?: Timestamp,
    name?: string,
    description?: string
  ) {
    super(manager, name, description);
    this.value = value;
    this.relativeTo = relativeTo;
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
