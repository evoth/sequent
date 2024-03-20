import { ActionState } from "./action";
import { Manageable, Manager, type IdType } from "./manager";
import { Render } from "./render";
import { Repeat, RepeatError, RepeatProps } from "./repeat";

import type { Repeatable } from "./repeat";
import type { CustomJSON, EntityManagers, Serializable } from "./serialization";

export type SequenceValidation = {
  error: SequenceError;
  childError?: LayerError;
  start?: number;
  end?: number;
};

export enum SequenceError {
  None,
  ChildError,
}

export class Sequence extends Manageable<Sequence> implements Repeatable {
  layers: Layer[];
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
    let sequenceStart: number | undefined = undefined;
    let sequenceEnd: number | undefined = undefined;
    for (const layer of this.layers) {
      const { error, childError, start, end } = layer.validate();
      if (error != LayerError.None) {
        return { error: SequenceError.ChildError, childError: error };
      }

      if (
        sequenceStart === undefined ||
        (start !== undefined &&
          sequenceStart !== undefined &&
          start < sequenceStart)
      ) {
        sequenceStart = start;
      }
      if (
        sequenceEnd === undefined ||
        (end !== undefined && sequenceEnd !== undefined && end > sequenceEnd)
      ) {
        sequenceEnd = end;
      }
    }

    return {
      error: SequenceError.None,
      start: sequenceStart,
      end: sequenceEnd,
    };
  }

  // Duration is undefined if sequence scope is infinite or if there are errors
  getDuration(keepLeading: boolean): number | undefined {
    const validation = this.validate();
    let duration;
    if (validation.start === undefined || validation.end === undefined) {
      duration = undefined;
    } else {
      duration = keepLeading
        ? validation.end
        : validation.end - validation.start;
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

  render(): Render {
    const render = new Render();
    for (const layer of this.layers) {
      render.add(layer.render());
      render.baseLayer = render.maxLayer + 1;
    }
    return render;
  }
}

export enum LayerMode {
  Coincide,
  Override,
}

export type LayerValidation = {
  error: LayerError;
  childError?: RepeatError;
  start?: number;
  end?: number;
  childBounds?: [start?: number, end?: number][];
};

export enum LayerError {
  None,
  Empty,
  ChildError,
  ChildOverlap,
}

export class Component extends Repeat implements Serializable {
  layerMode: LayerMode;
  customName?: string;

  constructor(
    child: Repeatable,
    props: RepeatProps,
    layerMode: LayerMode,
    customName?: string
  ) {
    super(child, props);
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
      json.customName
    );
  }
}

export class Layer implements Serializable {
  children: Set<Component>;

  constructor(children: Component[] = []) {
    this.children = new Set<Component>(children);
  }

  toJSON(): CustomJSON<Layer> {
    return {
      children: [...this.children],
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): Layer {
    return new Layer(
      json.children.map((componentJson: any) =>
        Component.fromJSON(componentJson, managers)
      )
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

    const childBounds: [start?: number, end?: number][] = [];
    for (const child of this.children) {
      if (child === excludeComponent) continue;
      const { error, solved } = child.validate();
      if (error !== RepeatError.None || solved === undefined) {
        return { error: LayerError.ChildError, childError: error };
      }
      childBounds.push([solved.start, solved.end]);
    }

    childBounds.sort((a, b) => {
      if (a[0] === undefined) return -1;
      if (b[0] === undefined) return 1;
      return a[0] - b[0];
    });

    let prevEnd: number | undefined = undefined;
    for (const [i, [start, end]] of childBounds.entries()) {
      if (
        i != 0 &&
        (start === undefined || prevEnd === undefined || start < prevEnd)
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
      duration = validation.end;
    }
    return [validation.error, duration];
  }

  render(): Render {
    const render = new Render();
    for (const child of this.children) {
      render.add(child.render());
    }
    return render;
  }
}
