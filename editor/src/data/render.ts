import { serializeRound } from "./serialization";

export type RenderChild = {
  start: number;
  end: number;
  layer: number;
  data: any;
};

export class Render {
  baseLayer: number = 0;
  maxLayer: number = 0;
  children: RenderChild[] = [];

  add(newRender: Render, offset: number = 0) {
    for (const child of newRender.children) {
      const newChild: RenderChild = {
        start: serializeRound(child.start + offset),
        end: serializeRound(child.end + offset),
        layer: child.layer + this.baseLayer,
        data: child.data,
      };
      this.maxLayer = Math.max(this.maxLayer, newChild.layer);
      this.children.push(newChild);
    }
    this.children.sort((a, b) => a.start - b.start);
  }

  export(isAbsolute: boolean): string {
    return JSON.stringify({
      numActions: this.children.length,
      maxLayer: this.maxLayer,
      isAbsolute: isAbsolute,
      actions: this.children,
    });
  }
}
