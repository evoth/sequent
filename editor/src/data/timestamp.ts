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

  getOffset(): [offset: number, absolute: Timestamp] {
    if (this.relativeTo === undefined) {
      return [0, this];
    }
    const [offset, absolute] = this.relativeTo.getOffset();
    return [offset + this.value, absolute];
  }
}
