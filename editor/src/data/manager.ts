import type {
  CustomJSON,
  EntityManagers,
  FromJSON,
  Serializable,
} from "./serialization";

export type IdType = string;

// Manages a list of a certain type of object, keeping track of their IDs
export class Manager<T extends Manageable<any>> implements Serializable {
  idCounter: number = 0;
  children: Map<IdType, T> = new Map<IdType, T>();

  toJSON(): {
    idCounter: number;
    children: T[];
  } {
    this.topologicalSort();
    // Serializes children as array because the order matters for proper deserialization
    return {
      idCounter: this.idCounter,
      children: [...this.children.values()],
    };
  }

  // Manager is a special case; its only job is to populate the already created object
  static fromJSON<T extends Manageable<any>>(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers,
    genericFromJSON: FromJSON<T>,
    newManager: Manager<T>
  ): Manager<T> {
    for (const child of json.children) {
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
    while (this.children.has(String(this.idCounter))) {
      this.idCounter++;
    }
    return String(this.idCounter++);
  }

  // TODO: Figure out why this hangs when data is messed up somehow
  findChildDependants(): Map<
    IdType,
    { dependencies: number; dependants: Set<IdType> }
  > {
    const graph = new Map<
      IdType,
      { dependencies: number; dependants: Set<IdType> }
    >();
    // "Reversing" the dependency graph
    for (const child of this.children.values()) {
      const ids = new Set(child.getChildIds());
      if (!graph.has(child.id)) {
        graph.set(child.id, {
          dependencies: ids.size,
          dependants: new Set<IdType>(),
        });
      }
      graph.get(child.id)!.dependencies = ids.size;

      for (const id of ids) {
        if (!graph.has(id)) {
          graph.set(id, { dependencies: -1, dependants: new Set<IdType>() });
        }
        graph.get(id)!.dependants.add(child.id);
      }
    }
    return graph;
  }

  topologicalSort(): void {
    const graph = this.findChildDependants();

    // Sort
    const sortedOrder: IdType[] = [];
    const ogLength = graph.size;
    for (let i = 0; i < ogLength; i++) {
      for (const [nodeId, node] of graph) {
        if (node.dependencies !== 0) continue;
        sortedOrder.push(nodeId);
        for (const id of node.dependants) {
          graph.get(id)!.dependencies--;
        }
        graph.delete(nodeId);
        break;
      }
    }

    if (sortedOrder.length != ogLength) {
      throw new Error(
        "No topological sort found for Manager children (circular dependency)."
      );
    }

    // Rearrange children in sorted order (Map objects retain order of insertion)
    this.children = new Map(
      sortedOrder.map((id) => [id, this.children.get(id)!])
    );
  }
}

// A hacky way to define "manageable" objects
export abstract class Manageable<T extends Manageable<T>>
  implements Serializable
{
  readonly id: IdType;
  readonly manager: Manager<T>;
  name: string;
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

  abstract add(): void;

  abstract getChildIds(): IdType[];

  getDependants(): T[] {
    return [...this.manager.findChildDependants().get(this.id)!.dependants].map(
      (id) => this.manager.children.get(id)!
    );
  }
}
