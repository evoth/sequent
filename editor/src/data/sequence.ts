import { ActionState } from "./action";
import { Manageable, Manager, type IdType } from "./manager";
import { Repeat, RepeatError, RepeatProps } from "./repeat";

import type { Repeatable } from "./repeat";
import type { CustomJSON, EntityManagers, Serializable } from "./serialization";
import { Timestamp } from "./timestamp";

export type SequenceValidation = {
  error: SequenceError;
  childError?: LayerError;
  start?: Timestamp;
  end?: Timestamp;
};

export enum SequenceError {
  None,
  ChildError,
  TimestampRootMismatch,
  WrongRootTimestamp,
}

export class Sequence extends Manageable<Sequence> implements Repeatable {
  layers: Layer[];
  // Undefined if root sequence
  // For right now, we don't support mixed layer types, so rootTimestamp is required
  rootTimestamp: Timestamp;
  isAbsolute: boolean;
  offset: number;
  scale: number;
  scroll: number;
  layerHeight: number;

  constructor(
    manager: Manager<Sequence>,
    name: string,
    description: string,
    layers: Layer[] = [],
    rootTimestamp: Timestamp,
    isAbsolute: boolean = false,
    offset: number = 0,
    scale: number = 10,
    scroll: number = 75,
    layerHeight: number = 150,
    id?: IdType,
    hue?: number
  ) {
    super(manager, name, description, id, hue);
    this.layers = layers;
    this.rootTimestamp = rootTimestamp;
    this.isAbsolute = isAbsolute;
    this.offset = offset;
    this.scale = scale;
    this.scroll = scroll;
    this.layerHeight = layerHeight;
  }

  toJSON(): CustomJSON<Sequence> {
    return {
      ...this.manageableJSON(),
      layers: this.layers,
      rootTimestamp: this.rootTimestamp?.id,
      isAbsolute: this.isAbsolute,
      offset: this.offset,
      scale: this.scale,
      scroll: this.scroll,
      layerHeight: this.layerHeight,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Sequence {
    return new Sequence(
      managers.sequenceManager,
      json.name,
      json.description,
      json.layers.map((layerJson: any) => Layer.fromJSON(layerJson, managers)),
      managers.timestampManager.children.get(json.rootTimestamp)!,
      json.isAbsolute,
      json.offset,
      json.scale,
      json.scroll,
      json.layerHeight,
      json.id,
      json.hue
    );
  }

  add() {
    this.manager.add(this);
  }

  validate(): SequenceValidation {
    // https://sungazermusic.bandcamp.com/track/sequence-start
    let sequenceStart: Timestamp | undefined = undefined;
    let sequenceEnd: Timestamp | undefined = undefined;
    for (const layer of this.layers) {
      const { error, childError, start, end } = layer.validate();
      if (error != LayerError.None) {
        return { error: SequenceError.ChildError, childError: error };
      }

      if (this.rootTimestamp !== undefined) {
        if (
          sequenceStart === undefined ||
          (start !== undefined &&
            sequenceStart !== undefined &&
            start.getOffset()[0] < sequenceStart.getOffset()[0])
        ) {
          sequenceStart = start;
        }
        if (
          sequenceEnd === undefined ||
          (end !== undefined &&
            sequenceEnd !== undefined &&
            end.getOffset()[0] > sequenceEnd.getOffset()[0])
        ) {
          sequenceEnd = end;
        }
      }

      if (layer.rootTimestamp !== this.rootTimestamp) {
        return { error: SequenceError.WrongRootTimestamp };
      }
    }

    return {
      error: SequenceError.None,
      start: sequenceStart,
      end: sequenceEnd,
    };
  }

  // Duration is undefined if sequence scope is infinite or if there are errors
  getDuration(): number | undefined {
    const validation = this.validate();
    let duration;
    if (validation.start === undefined || validation.end === undefined) {
      duration = undefined;
    } else {
      duration =
        validation.end.getOffset()[0] - validation.start.getOffset()[0];
    }
    return duration;
  }

  getChildIds(): IdType[] {
    let childIds: IdType[] = [];
    for (const layer of this.layers) {
      for (const component of layer.children) {
        if (component.child instanceof Sequence) {
          childIds.push(component.child.id);
        }
      }
    }
    return childIds;
  }

  getManageableChild(): Manageable<Sequence> {
    return this;
  }
}

export enum LayerMode {
  Coincide,
  Override,
}

export type LayerValidation = {
  error: LayerError;
  childError?: RepeatError;
  start?: Timestamp;
  end?: Timestamp;
  childBounds?: [start?: Timestamp, end?: Timestamp][];
};

export enum LayerError {
  None,
  Empty,
  ChildError,
  TimestampRootMismatch,
  WrongRootTimestamp,
  ChildOverlap,
}

export class Component extends Repeat implements Serializable {
  layerMode: LayerMode;
  customName?: string;

  constructor(
    child: Repeatable,
    props: RepeatProps,
    layerMode: LayerMode,
    rootTimestamp?: Timestamp,
    customName?: string
  ) {
    super(child, props, rootTimestamp);
    this.layerMode = layerMode;
    this.customName = customName;
  }

  toJSON(): CustomJSON<Component> {
    return {
      ...this.repeatJSON(),
      // TODO: make sure enum conversion works correctly
      layerMode: LayerMode[this.layerMode],
      customName: this.customName,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Component {
    let child: Repeatable;
    if (json.child.type === "Action") {
      child = ActionState.fromJSON(json.child.json, managers);
    } else if (json.child.type === "Sequence") {
      child = managers.sequenceManager.children.get(json.child.id)!;
    } else {
      throw new Error(
        "Unexpected value for 'child.type' when deserializing Component."
      );
    }
    return new Component(
      child,
      RepeatProps.fromJSON(json.props, managers),
      // TODO: make sure enum conversion works correctly
      LayerMode[json.layerMode as keyof typeof LayerMode],
      managers.timestampManager.children.get(json.rootTimestamp),
      json.customName
    );
  }
}

export class Layer implements Serializable {
  children: Set<Component>;
  // If sequence passes in root timestamp, all timestamps must descend from it.
  // Otherwise (as in the case of the root sequence), any timestamp ancestor
  // is fine as long as child timestamps are consistent with each other.
  rootTimestamp?: Timestamp;

  constructor(children: Component[] = [], rootTimestamp?: Timestamp) {
    this.children = new Set<Component>(children);
    this.rootTimestamp = rootTimestamp;
  }

  toJSON(): CustomJSON<Layer> {
    return {
      children: [...this.children],
      rootTimestamp: this.rootTimestamp?.id,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Layer {
    return new Layer(
      json.children.map((componentJson: any) =>
        Component.fromJSON(componentJson, managers)
      ),
      managers.timestampManager.children.get(json.rootTimestamp)
    );
  }

  validate(excludeComponent?: Component): LayerValidation {
    if (
      this.children.size === 0 ||
      (this.children.size === 1 &&
        this.children.values().next().value === excludeComponent)
    ) {
      return { error: LayerError.Empty };
    }

    const childBounds: [start?: Timestamp, end?: Timestamp][] = [];
    const rootTimestamps = new Set<Timestamp>();
    for (const child of this.children) {
      if (child === excludeComponent) continue;
      const { error, solved } = child.validate();
      if (error !== RepeatError.None || solved === undefined) {
        return { error: LayerError.ChildError, childError: error };
      }
      childBounds.push([solved.start, solved.end]);
      const [offset, root] = (solved.start ?? solved.end)!.getOffset();
      rootTimestamps.add(root);
    }

    if (
      this.rootTimestamp !== undefined &&
      rootTimestamps.values().next().value !== this.rootTimestamp
    ) {
      return { error: LayerError.WrongRootTimestamp };
    }
    if (rootTimestamps.size > 1) {
      return { error: LayerError.TimestampRootMismatch };
    }

    childBounds.sort((a, b) => {
      if (a[0] === undefined) return -1;
      if (b[0] === undefined) return 1;
      return a[0].getOffset()[0] - b[0].getOffset()[0];
    });

    let prevEnd: Timestamp | undefined = undefined;
    for (const [i, [start, end]] of childBounds.entries()) {
      if (
        i != 0 &&
        (start === undefined ||
          prevEnd === undefined ||
          start.getOffset()[0] < prevEnd.getOffset()[0])
      ) {
        return { error: LayerError.ChildOverlap };
      }
      prevEnd = end;
    }

    return {
      error: LayerError.None,
      start: childBounds[0][0],
      end: prevEnd,
      childBounds: childBounds,
    };
  }

  getDuration(): [error: LayerError, duration?: number] {
    const validation = this.validate();
    let duration;
    if (validation.start === undefined || validation.end === undefined) {
      duration = undefined;
    } else {
      duration =
        validation.end.getOffset()[0] - validation.start.getOffset()[0];
    }
    return [validation.error, duration];
  }
}
