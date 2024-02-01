export class Timestamp {
  id: number;
  value: number;
  relativeTo?: Timestamp;

  static readonly StartId = -1;

  constructor(id: number, value: number, relativeTo?: Timestamp) {
    this.id = id;
    this.value = value;
    this.relativeTo = relativeTo;
  }

  getOffset(): [offset: number, root: Timestamp] {
    if (this.relativeTo === undefined) {
      return [0, this];
    }
    const [offset, root] = this.relativeTo.getOffset();
    return [offset + this.value, root];
  }
}
