export class Timestamp {
  id: number;
  value: number;
  relativeTo?: Timestamp;

  private static idCounter = 0;

  constructor(value: number, relativeTo?: Timestamp) {
    this.id = Timestamp.generateId();
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

  static generateId(): number {
    return this.idCounter++;
  }
}
