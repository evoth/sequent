// All durations in seconds
class Timescale {
  readonly titleInterval: number | undefined;
  readonly labelInterval: number;
  readonly getTitle: (offset: number) => string;
  readonly getLabel: (offset: number) => string;

  constructor(
    titleInterval: number | undefined,
    labelInterval: number,
    getTitle: (offset: number) => string,
    getLabel: (offset: number) => string
  ) {
    this.titleInterval = titleInterval;
    this.labelInterval = labelInterval;
    this.getTitle = getTitle;
    this.getLabel = getLabel;
  }

  private getIntervals(
    start: number,
    end: number,
    interval: number | undefined,
    getLabel: (offset: number) => string
  ): [offset: number, label: string][] {
    if (interval === undefined) return [];

    const intervals: [number, string][] = [];
    for (
      let i = Math.floor(start / interval);
      i < Math.ceil(end / interval);
      i++
    ) {
      const intervalOffset = i * interval;
      if (interval >= start && interval < end) {
        intervals.push([intervalOffset, getLabel(intervalOffset)]);
      }
    }
    return intervals;
  }

  getTitleIntervals(
    start: number,
    end: number
  ): [offset: number, label: string][] {
    return this.getIntervals(start, end, this.titleInterval, this.getTitle);
  }

  getLabelIntervals(
    start: number,
    end: number
  ): [offset: number, label: string][] {
    return this.getIntervals(start, end, this.labelInterval, this.getLabel);
  }
}

class TimescaleSet {
  timescales: Timescale[];

  constructor(timescales: Timescale[]) {
    this.timescales = timescales;
  }

  // If labelWidth is px and scale is px/s, returns Timescale whose labels
  // will be closest to labelWidth px. Same applies to any unit of length.
  bestTimescale(labelWidth: number, scale: number): Timescale {
    let minDiff: number | undefined = undefined;
    let best: Timescale | undefined = undefined;

    for (const timescale of this.timescales) {
      const widthDiff = Math.abs(timescale.labelInterval * scale - labelWidth);
      if (minDiff === undefined || widthDiff < minDiff) {
        minDiff = widthDiff;
        best = timescale;
      }
    }

    if (best === undefined) {
      throw new Error(
        "No best timescale found. Make sure timescales array is not empty."
      );
    }
    return best;
  }
}

// TODO: clean up / standardize / improve (this is just for testing)
const levelLabel = (
  level: number,
  lastOnly: boolean = false
): ((offset: number) => string) => {
  return (offset: number) => {
    const levels: [number, string][] = [
      [86400, "d"],
      [3600, "h"],
      [60, "m"],
      [1, "s"],
    ];
    const labels: string[] = [];
    let flag = false;
    for (let i = 0; i < level; i++) {
      const [interval, unit] = levels[i];
      const num = Math.floor(offset / interval);
      flag = flag || num > 0;
      labels.push(flag ? `${num}${unit}` : "");
      offset %= interval;
    }
    return lastOnly ? labels[level - 1] : labels.join("");
  };
};

export const RelativeTimescales = new TimescaleSet([
  new Timescale(undefined, 86400, (offset) => "", levelLabel(1, true)),
  new Timescale(86400, 6 * 3600, levelLabel(1), levelLabel(2, true)),
  new Timescale(86400, 2 * 3600, levelLabel(1), levelLabel(2, true)),
  new Timescale(86400, 3600, levelLabel(1), levelLabel(2, true)),
  new Timescale(3600, 15 * 60, levelLabel(2), levelLabel(3, true)),
  new Timescale(3600, 5 * 60, levelLabel(2), levelLabel(3, true)),
  new Timescale(3600, 60, levelLabel(2), levelLabel(3, true)),
  new Timescale(60, 15 * 1, levelLabel(3), levelLabel(4, true)),
  new Timescale(60, 5 * 1, levelLabel(3), levelLabel(4, true)),
  new Timescale(60, 1, levelLabel(3), levelLabel(4, true)),
]);

// TODO: Add AbsoluteTimescale that overrides the getIntervals function with something that works with DateTimes
// TODO: Add setting for MM/DD vs DD/MM
