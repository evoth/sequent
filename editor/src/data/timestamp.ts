import { Manageable, Manager, type IdType } from "./manager";
import type { CustomJSON, EntityManagers } from "./serialization";

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

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Timestamp {
    return new Timestamp(
      managers.timestampManager,
      json.value,
      managers.timestampManager.children.get(json.relativeTo),
      json.name,
      json.description,
      json.id,
      json.hue
    );
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

  getChildIds(): IdType[] {
    return this.relativeTo !== undefined ? [this.relativeTo.id] : [];
  }
}
