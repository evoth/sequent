import { Manageable, Manager, type IdType } from "./manager";
import type { CustomJSON, EntityManagers } from "./serialization";

export class Timestamp extends Manageable<Timestamp> {
  value: number;
  relativeTo?: Timestamp;
  // Only matters if timestamp is root timestamp
  isAbsolute: boolean;

  constructor(
    manager: Manager<Timestamp>,
    value: number,
    isAbsolute: boolean = false,
    relativeTo?: Timestamp,
    // TODO: fix it so that name can be undefined?
    name: string = "",
    description?: string,
    id?: IdType,
    hue?: number,
    addToManager?: boolean
  ) {
    super(manager, name, description, id, hue, addToManager);
    this.value = value;
    this.relativeTo = relativeTo;
    this.isAbsolute = isAbsolute;
  }

  toJSON(): CustomJSON<Timestamp> {
    return {
      ...this.manageableJSON(),
      value: this.value,
      isAbsolute: this.isAbsolute,
      relativeTo: this.relativeTo?.id,
      offset: null,
      root: null,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Timestamp {
    return new Timestamp(
      managers.timestampManager,
      json.value,
      json.isAbsolute,
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

  get offset(): number {
    return this.getOffset()[0];
  }

  set offset(newOffset: number) {
    this.value = newOffset - (this.offset - this.value);
  }

  get root(): Timestamp {
    return this.getOffset()[1];
  }

  getChildIds(): IdType[] {
    return this.relativeTo !== undefined ? [this.relativeTo.id] : [];
  }
}
