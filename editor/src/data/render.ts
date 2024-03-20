export type RenderChild = {
  start: number;
  end: number;
  layer: number;
  data: any;
};

const EXPORT_ROUND_DIGITS = 6;

function exportRound(num: number) {
  // https://stackoverflow.com/a/11832950
  return (
    Math.round((num + Number.EPSILON) * 10 ** EXPORT_ROUND_DIGITS) /
    10 ** EXPORT_ROUND_DIGITS
  );
}

export class Render {
  baseLayer: number = 0;
  maxLayer: number = 0;
  children: RenderChild[] = [];

  add(newRender: Render, offset: number = 0) {
    for (const child of newRender.children) {
      const newChild: RenderChild = {
        start: exportRound(child.start + offset),
        end: exportRound(child.end + offset),
        layer: child.layer + this.baseLayer,
        data: child.data,
      };
      this.maxLayer = Math.max(this.maxLayer, newChild.layer);
      this.children.push(newChild);
    }
    this.children.sort((a, b) => a.start - b.start);
  }

  export(): string {
    return JSON.stringify({
      numActions: this.children.length,
      maxLayer: this.maxLayer,
      actions: this.children,
    });
  }
}
