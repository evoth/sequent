export type RenderChild = {
  start: number;
  end: number;
  layer: number;
  data: any;
};

export class Render {
  baseLayer: number = 0;
  maxLayer: number = 0;
  private children: RenderChild[] = [];

  add(newRender: Render, offset: number = 0) {
    for (const child of newRender.children) {
      const newChild: RenderChild = {
        start: child.start + Math.round(offset * 1000),
        end: child.end + Math.round(offset * 1000),
        layer: child.layer + this.baseLayer,
        data: child.data,
      };
      this.maxLayer = Math.max(this.maxLayer, newChild.layer);
      this.children.push(newChild);
    }
    this.children.sort((a, b) => a.start - b.start);
  }

  addChild(start: number, end: number, layer: number, data: any) {
    this.children.push({
      start: Math.round(start * 1000),
      end: Math.round(end * 1000),
      layer: layer,
      data: data,
    });
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
