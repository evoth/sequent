// All durations in seconds
export class Timescale {
  readonly titleInterval: number | undefined;
  readonly labelInterval: number;
  readonly tileInterval: number;
  readonly getTitle: (offset: number) => string;
  readonly getLabel: (offset: number) => string;

  constructor(
    titleInterval: number | undefined,
    labelInterval: number,
    tileInterval: number,
    getTitle: (offset: number) => string,
    getLabel: (offset: number) => string
  ) {
    this.titleInterval = titleInterval;
    this.labelInterval = labelInterval;
    this.tileInterval = tileInterval;
    this.getTitle = getTitle;
    this.getLabel = getLabel;
  }

  getIntervals<T>(
    start: number,
    end: number,
    interval: number | undefined,
    getLabel: (offset: number) => T,
    includeStart: boolean = false
  ): [offset: number, label: T][] {
    if (interval === undefined) return [];

    const intervals: [number, T][] = [];
    const floor = Math.floor(start / interval);
    const ceil = Math.ceil(end / interval);
    for (let i = floor; i < ceil; i++) {
      const intervalOffset = i * interval;
      if (
        (intervalOffset >= start && intervalOffset < end) ||
        (includeStart && i == floor)
      ) {
        intervals.push([intervalOffset, getLabel(intervalOffset)]);
      }
    }
    return intervals;
  }

  getTitleIntervals(
    start: number,
    end: number
  ): [offset: number, label: string][] {
    return this.getIntervals(
      start,
      end,
      this.titleInterval,
      this.getTitle,
      true
    );
  }

  getLabelIntervals(
    start: number,
    end: number
  ): [offset: number, label: string][] {
    return this.getIntervals(
      start,
      end,
      this.labelInterval,
      this.getLabel,
      false
    );
  }

  getTileIntervals(
    start: number,
    end: number
  ): [offset: number, index: number][] {
    return this.getIntervals(
      start,
      end,
      this.tileInterval,
      (offset: number) => offset / this.tileInterval,
      true
    );
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

  shiftIndex(reference: Timescale, shift: number): Timescale {
    const index = this.timescales.indexOf(reference);
    if (index === 0) {
      return this.timescales[0];
    } else if (index === this.timescales.length - 1) {
      return this.timescales[this.timescales.length - 1];
    }
    return this.timescales[index + shift];
  }
}

// TODO: clean up / standardize / improve (this is just for testing)
// TODO: and remember to add 1 and 5 times powers of ten on either side
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
      labels.push(flag || i == level - 1 ? `${num}${unit}` : "");
      offset %= interval;
    }
    return lastOnly ? labels[level - 1] : labels.join("");
  };
};

export const RelativeTimescales = new TimescaleSet([
  new Timescale(
    undefined,
    86400,
    2 * 86400,
    (offset) => "",
    levelLabel(1, true)
  ),
  new Timescale(86400, 6 * 3600, 86400, levelLabel(1), levelLabel(2, true)),
  new Timescale(86400, 2 * 3600, 6 * 3600, levelLabel(1), levelLabel(2, true)),
  new Timescale(86400, 3600, 2 * 3600, levelLabel(1), levelLabel(2, true)),
  new Timescale(3600, 15 * 60, 3600, levelLabel(2), levelLabel(3, true)),
  new Timescale(3600, 5 * 60, 15 * 60, levelLabel(2), levelLabel(3, true)),
  new Timescale(3600, 60, 5 * 60, levelLabel(2), levelLabel(3, true)),
  new Timescale(60, 15 * 1, 60, levelLabel(3), levelLabel(4, true)),
  new Timescale(60, 5 * 1, 15 * 1, levelLabel(3), levelLabel(4, true)),
  new Timescale(60, 1, 5 * 1, levelLabel(3), levelLabel(4, true)),
]);

// TODO: Add AbsoluteTimescale that overrides the getIntervals function with something that works with DateTimes
// TODO: Add setting for MM/DD vs DD/MM
