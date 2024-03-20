export const LABEL_TIMESCALE_PX = 360;

const Units = {
  DAY: 86400,
  HOUR: 3600,
  MINUTE: 60,
  SECOND: 1,
  YEAR: 365 * 86400,
  MONTH: 30 * 86400,
  WEEK: 7 * 86400,
  DAY_MWF: (7 / 3) * 86400,
};

// All durations in seconds
export class Timescale {
  readonly titleInterval: [unit: number, amount: number] | undefined;
  readonly labelInterval: [unit: number, amount: number];
  readonly tileInterval: [unit: number, amount: number];
  readonly getTitle: (offset: number) => string;
  readonly getLabel: (offset: number) => string;

  constructor(
    titleInterval: [unit: number, amount: number] | undefined,
    labelInterval: [unit: number, amount: number],
    tileInterval: [unit: number, amount: number],
    getTitle: (offset: number) => string,
    getLabel: (offset: number) => string
  ) {
    this.titleInterval = titleInterval;
    this.labelInterval = labelInterval;
    this.tileInterval = tileInterval;
    this.getTitle = getTitle;
    this.getLabel = getLabel;
  }

  intervalDuration(interval: [unit: number, amount: number]): number {
    return interval[0] * interval[1];
  }

  getIntervals<T>(
    start: number,
    end: number,
    interval: [unit: number, amount: number] | undefined,
    getLabel: (offset: number) => T,
    includeStart: boolean = false
  ): [offset: number, label: T, duration: number][] {
    if (interval === undefined) return [];
    const intervalDuration = this.intervalDuration(interval);

    const intervals: [number, T, number][] = [];
    const floor = Math.floor(start / intervalDuration);
    const ceil = Math.ceil(end / intervalDuration);
    for (let i = floor; i < ceil; i++) {
      const intervalOffset = i * intervalDuration;
      if (
        (intervalOffset >= start && intervalOffset < end) ||
        (includeStart && i == floor)
      ) {
        intervals.push([
          intervalOffset,
          getLabel(intervalOffset),
          intervalDuration,
        ]);
      }
    }
    return intervals;
  }

  getTitleIntervals(
    start: number,
    end: number
  ): [offset: number, label: string, duration: number][] {
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
  ): [offset: number, label: string, duration: number][] {
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
  ): [offset: number, index: number, duration: number][] {
    return this.getIntervals(
      start,
      end,
      this.tileInterval,
      (offset: number) => offset / this.intervalDuration(this.tileInterval),
      true
    );
  }
}

export class TimescaleSet {
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
      const widthDiff = Math.abs(
        timescale.intervalDuration(timescale.labelInterval) * scale - labelWidth
      );
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
    const index = this.timescales.indexOf(reference) + shift;
    if (index <= 0) {
      return this.timescales[0];
    } else if (index >= this.timescales.length - 1) {
      return this.timescales[this.timescales.length - 1];
    }
    return this.timescales[index];
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
    [Units.DAY, 1],
    [Units.DAY, 2],
    (offset) => "",
    levelLabel(1, true)
  ),
  new Timescale(
    [Units.DAY, 1],
    [Units.HOUR, 6],
    [Units.DAY, 1],
    levelLabel(1),
    levelLabel(2, true)
  ),
  new Timescale(
    [Units.DAY, 1],
    [Units.HOUR, 2],
    [Units.HOUR, 6],
    levelLabel(1),
    levelLabel(2, true)
  ),
  new Timescale(
    [Units.DAY, 1],
    [Units.HOUR, 1],
    [Units.HOUR, 2],
    levelLabel(1),
    levelLabel(2, true)
  ),
  new Timescale(
    [Units.HOUR, 1],
    [Units.MINUTE, 15],
    [Units.HOUR, 1],
    levelLabel(2),
    levelLabel(3, true)
  ),
  new Timescale(
    [Units.HOUR, 1],
    [Units.MINUTE, 5],
    [Units.MINUTE, 15],
    levelLabel(2),
    levelLabel(3, true)
  ),
  new Timescale(
    [Units.HOUR, 1],
    [Units.MINUTE, 1],
    [Units.MINUTE, 5],
    levelLabel(2),
    levelLabel(3, true)
  ),
  new Timescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 15],
    [Units.MINUTE, 1],
    levelLabel(3),
    levelLabel(4, true)
  ),
  new Timescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 5],
    [Units.SECOND, 15],
    levelLabel(3),
    levelLabel(4, true)
  ),
  new Timescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 1],
    [Units.SECOND, 5],
    levelLabel(3),
    levelLabel(4, true)
  ),
]);

// TODO: Add setting for MM/DD vs DD/MM
export class AbsoluteTimescale extends Timescale {
  private dateIntervalStartFunctions = new Map([
    [
      Units.YEAR,
      (date: Date, amount: number) =>
        new Date(date.getFullYear() - (date.getFullYear() % amount), 0),
    ],
    [
      Units.MONTH,
      (date: Date, amount: number) =>
        new Date(
          date.getFullYear(),
          date.getMonth() - (date.getMonth() % amount)
        ),
    ],
    [
      Units.WEEK,
      (date: Date, amount: number) =>
        new Date(
          date.getFullYear(),
          date.getMonth(),
          date.getDate() - date.getDay() + (date.getDay() === 0 ? -6 : 1)
        ),
    ],
    [
      Units.DAY_MWF,
      (date: Date, amount: number) => {
        date.setHours(0, 0, 0, 0);
        while (![1, 3, 5].includes(date.getDay())) {
          date.setDate(date.getDate() - 1);
        }
        return date;
      },
    ],
    [
      Units.DAY,
      (date: Date, amount: number) => {
        date.setHours(0, 0, 0, 0);
        return date;
      },
    ],
    [
      Units.HOUR,
      (date: Date, amount: number) => {
        date.setHours(date.getHours() - (date.getHours() % amount), 0, 0, 0);
        return date;
      },
    ],
    [
      Units.MINUTE,
      (date: Date, amount: number) => {
        date.setMinutes(date.getMinutes() - (date.getMinutes() % amount), 0, 0);
        return date;
      },
    ],
    [
      Units.SECOND,
      (date: Date, amount: number) => {
        date.setSeconds(date.getSeconds() - (date.getSeconds() % amount), 0);
        return date;
      },
    ],
  ]);

  private dateIncrementFunctions = new Map([
    [
      Units.YEAR,
      (date: Date, amount: number) => {
        date.setFullYear(date.getFullYear() + amount);
        return date;
      },
    ],
    [
      Units.MONTH,
      (date: Date, amount: number) => {
        date.setMonth(date.getMonth() + amount);
        return date;
      },
    ],
    [
      Units.WEEK,
      (date: Date, amount: number) => {
        date.setDate(date.getDate() + 7 * amount);
        return date;
      },
    ],
    [
      Units.DAY_MWF,
      (date: Date, amount: number) => {
        date.setDate(date.getDate() + 1);
        while (![1, 3, 5].includes(date.getDay())) {
          date.setDate(date.getDate() + 1);
        }
        return date;
      },
    ],
    [
      Units.DAY,
      (date: Date, amount: number) => {
        date.setDate(date.getDate() + amount);
        return date;
      },
    ],
    [
      Units.HOUR,
      (date: Date, amount: number) => {
        date.setHours(date.getHours() + amount);
        return date;
      },
    ],
    [
      Units.MINUTE,
      (date: Date, amount: number) => {
        date.setMinutes(date.getMinutes() + amount);
        return date;
      },
    ],
    [
      Units.SECOND,
      (date: Date, amount: number) => {
        date.setSeconds(date.getSeconds() + amount);
        return date;
      },
    ],
  ]);

  getIntervals<T>(
    start: number,
    end: number,
    interval: [unit: number, amount: number] | undefined,
    getLabel: (offset: number) => T,
    includeStart: boolean = false
  ): [offset: number, label: T, duration: number][] {
    if (interval === undefined) return [];

    const intervals: [number, T, number][] = [];
    const startDateFunction = this.dateIntervalStartFunctions.get(interval[0]);
    const incrementDateFunction = this.dateIncrementFunctions.get(interval[0]);
    if (
      startDateFunction === undefined ||
      incrementDateFunction === undefined
    ) {
      throw new Error(
        `Unknown absolute timestamp interval: ${interval[0]} seconds`
      );
    }

    const startDate = startDateFunction(new Date(start * 1000), interval[1]);
    const startOffset = startDate.getTime() / 1000;
    let intervalDate = new Date(startDate.getTime());
    let intervalOffset = intervalDate.getTime() / 1000;

    while (intervalOffset < end) {
      if (
        (intervalOffset >= start && intervalOffset < end) ||
        (includeStart && intervalOffset == startOffset)
      ) {
        intervals.push([intervalOffset, getLabel(intervalOffset), 0]);
      }
      intervalDate = incrementDateFunction(intervalDate, interval[1]);
      intervalOffset = intervalDate.getTime() / 1000;
    }
    intervals.push([intervalOffset, getLabel(intervalOffset), 0]);

    for (let i = 0; i < intervals.length - 1; i++) {
      intervals[i][2] = intervals[i + 1][0] - intervals[i][0];
    }
    intervals.pop();

    return intervals;
  }
}

// TODO: clean up / standardize / improve (this is just for testing)
// TODO: and remember to add 1 and 5 times powers of ten on either side
function dateFormat(transform: (date: Date) => number | string) {
  return (offset: number) => transform(new Date(offset * 1000)).toString();
}

export const AbsoluteTimescales = new TimescaleSet([
  new AbsoluteTimescale(
    undefined,
    [Units.YEAR, 1],
    [Units.YEAR, 2],
    (offset) => "",
    dateFormat((date) => date.getFullYear())
  ),
  new AbsoluteTimescale(
    [Units.YEAR, 1],
    [Units.MONTH, 3],
    [Units.YEAR, 1],
    dateFormat((date) => date.getFullYear()),
    dateFormat((date) => date.toLocaleDateString("default", { month: "short" }))
  ),
  new AbsoluteTimescale(
    [Units.YEAR, 1],
    [Units.MONTH, 1],
    [Units.MONTH, 3],
    dateFormat((date) => date.getFullYear()),
    dateFormat((date) => date.toLocaleDateString("default", { month: "short" }))
  ),
  new AbsoluteTimescale(
    [Units.MONTH, 1],
    [Units.WEEK, 1],
    [Units.MONTH, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
      })
    ),
    // TODO: Add setting for MM/DD vs DD/MM
    dateFormat((date) => `${date.getMonth() + 1}/${date.getDate()}`)
  ),
  new AbsoluteTimescale(
    [Units.MONTH, 1],
    [Units.DAY_MWF, 1],
    [Units.WEEK, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
      })
    ),
    dateFormat((date) => `${date.getMonth() + 1}/${date.getDate()}`)
  ),
  new AbsoluteTimescale(
    [Units.MONTH, 1],
    [Units.DAY, 1],
    [Units.DAY_MWF, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
      })
    ),
    dateFormat((date) => `${date.getMonth() + 1}/${date.getDate()}`)
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.HOUR, 6],
    [Units.DAY, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.HOUR, 2],
    [Units.HOUR, 6],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.HOUR, 1],
    [Units.HOUR, 2],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.MINUTE, 15],
    [Units.HOUR, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
        minute: "2-digit",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.MINUTE, 5],
    [Units.MINUTE, 15],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
        minute: "2-digit",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.DAY, 1],
    [Units.MINUTE, 1],
    [Units.MINUTE, 5],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        hour: "numeric",
        minute: "2-digit",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 15],
    [Units.MINUTE, 1],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "numeric",
        minute: "2-digit",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        second: "numeric",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 5],
    [Units.SECOND, 15],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "numeric",
        minute: "2-digit",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        second: "numeric",
      })
    )
  ),
  new AbsoluteTimescale(
    [Units.MINUTE, 1],
    [Units.SECOND, 1],
    [Units.SECOND, 5],
    dateFormat((date) =>
      date.toLocaleDateString("default", {
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "numeric",
        minute: "2-digit",
      })
    ),
    dateFormat((date) =>
      date.toLocaleTimeString("default", {
        second: "numeric",
      })
    )
  ),
]);
