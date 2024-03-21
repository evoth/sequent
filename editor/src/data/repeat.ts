import type { IdType, Manageable } from "./manager";
import {
  serializeRound,
  type CustomJSON,
  type EntityManagers,
  type Serializable,
} from "./serialization";

import { ActionState } from "./action";
import { Render } from "./render";
import { Sequence } from "./sequence";

export interface Repeatable {
  // An undefined duration means infinite
  getDuration(keepLeading: boolean): number | undefined;
  getManageableChild(): Manageable<any>;
  render(): Render;
}

export class Repeat implements Serializable {
  child: Repeatable;
  props: RepeatProps;

  constructor(child: Repeatable, props: RepeatProps) {
    this.child = child;
    this.props = props;
  }

  toJSON(): CustomJSON<Repeat> {
    return this.repeatJSON();
  }

  repeatJSON(): CustomJSON<Repeat> {
    let childJSON: { type: string; json?: Serializable; id?: IdType };
    if (this.child instanceof ActionState) {
      childJSON = { type: "Action", json: this.child };
    } else if (this.child instanceof Sequence) {
      childJSON = { type: "Sequence", id: this.child.id };
    } else {
      throw new Error("Cannot serialize unknown Repeatable subclass.");
    }
    return {
      child: childJSON,
      props: this.props,
    };
  }

  validate(): RepeatValidation {
    const childDuration = this.child.getDuration(this.props.keepLeading);
    if (childDuration === undefined)
      return { error: RepeatError.ChildDurationInfinite };
    return this.props.validate(childDuration);
  }

  getDuration(): [error: RepeatError, duration?: number] {
    const childDuration = this.child.getDuration(this.props.keepLeading);
    if (childDuration === undefined) return [RepeatError.ChildDurationInfinite];

    const { error, solved } = this.props.validate(childDuration);
    return [error, solved?.duration];
  }
}

export type RepeatValidation = {
  error: RepeatError;
  solved?: SolvedRepeatConstraints;
};

export enum RepeatError {
  None,
  WrongNumConstraints,
  InvalidConstraints,
  EmptyConstraints,
  EndBeforeStart,
  NegativeDuration,
  TooFewRepetitions,
  FractionalRepetitions,
  IntervalTooShort,
  DurationTooShort,
  ChildDurationInfinite,
}

export type RepeatConstraints = {
  start?: number;
  end?: number;
  duration?: number;
  repetitions?: number;
  interval?: number;
};

export type SolvedRepeatConstraints = {
  start?: number;
  end?: number;
  duration?: number;
  repetitions: number;
  interval: number;
};

// Represents a tree of valid constraints. For each step, the keys are the
// valid values and the values are lists of valid constraints that can follow.
// TODO: Allow infinite repetitions and anchoring by end time
export const validConstraints: {
  [key in keyof RepeatConstraints]?: (keyof RepeatConstraints)[];
}[] = [
  {
    start: ["end", "duration", "repetitions"], //, "interval"],
    // end: ["duration", "repetitions", "interval"],
  },
  {
    end: ["repetitions", "interval"],
    duration: ["repetitions", "interval"],
    repetitions: ["interval"],
    interval: [],
  },
];

export type RepeatRelativeUnitType =
  | "seconds"
  | "minutes"
  | "hours"
  | "HH:MM:SS";

export type RepeatAbsoluteUnitType = "YYYY-MM-DD HH:MM:SS" | "unix time";

export type RepeatUnitType = RepeatRelativeUnitType | RepeatAbsoluteUnitType;

// Describes how a repeatable object is to be repeated and provides in-depth
// validations of properties
export class RepeatProps implements Serializable {
  constraints: RepeatConstraints;
  selectedConstraints: (keyof RepeatConstraints | undefined)[];
  includeChildDuration: boolean;
  trailingInterval: boolean;
  keepLeading: boolean;
  unit: RepeatUnitType;
  durationUnit: RepeatRelativeUnitType;

  constructor(
    constraints: RepeatConstraints,
    selectedConstraints: (keyof RepeatConstraints | undefined)[],
    includeChildDuration: boolean = false,
    trailingInterval: boolean = false,
    keepLeading: boolean = false,
    unit: RepeatUnitType = "seconds",
    durationUnit: RepeatRelativeUnitType = "seconds"
  ) {
    this.constraints = constraints;
    this.selectedConstraints = selectedConstraints;
    this.includeChildDuration = includeChildDuration;
    this.trailingInterval = trailingInterval;
    this.keepLeading = keepLeading;
    this.unit = unit;
    this.durationUnit = durationUnit;
  }

  toJSON(): CustomJSON<RepeatProps> {
    return {
      constraints: {
        start: this.constraints.start,
        end: this.constraints.end,
        duration: this.constraints.duration,
        repetitions: this.constraints.repetitions,
        interval: this.constraints.interval,
      },
      selectedConstraints: this.selectedConstraints,
      includeChildDuration: this.includeChildDuration,
      trailingInterval: this.trailingInterval,
      keepLeading: this.keepLeading,
      unit: this.unit,
      durationUnit: this.durationUnit,
    };
  }

  static fromJSON(
    json: ReturnType<typeof this.prototype.toJSON>,
    managers: EntityManagers
  ): RepeatProps {
    return new RepeatProps(
      {
        start: json.constraints.start,
        end: json.constraints.end,
        duration: json.constraints.duration,
        repetitions: json.constraints.repetitions,
        interval: json.constraints.interval,
      },
      json.selectedConstraints,
      json.includeChildDuration,
      json.trailingInterval,
      json.keepLeading,
      json.unit,
      json.durationUnit
    );
  }

  validateConstraints(): RepeatError {
    if (this.selectedConstraints.length !== validConstraints.length + 1) {
      return RepeatError.WrongNumConstraints;
    }
    // Checking validity against the "tree" of valid combinations
    for (let i = 0; i < validConstraints.length; i++) {
      const constraint = this.selectedConstraints[i];
      if (constraint === undefined || !(constraint in validConstraints[i])) {
        return RepeatError.InvalidConstraints;
      }
      // Gets the options that are allowed to follow this one
      const nextConstraintOptions = validConstraints[i][constraint];
      if (nextConstraintOptions === undefined) {
        return RepeatError.InvalidConstraints;
      }
      // Get the following option in the input
      const nextConstraint = this.selectedConstraints[i + 1];
      // Undefined is valid only if the options list is empty
      if (nextConstraint === undefined) {
        if (nextConstraintOptions.length === 0) {
          break;
        } else {
          return RepeatError.InvalidConstraints;
        }
      }
      // If the options list isn't empty, the following option must be in it
      if (!nextConstraintOptions.includes(nextConstraint)) {
        return RepeatError.InvalidConstraints;
      }
    }
    return RepeatError.None;
  }

  isSelected(constraintName: keyof RepeatConstraints | undefined) {
    return this.selectedConstraints.includes(constraintName);
  }

  validate(childDuration: number): RepeatValidation {
    const constraintError = this.validateConstraints();
    if (constraintError !== RepeatError.None) {
      return { error: constraintError };
    }

    // Selected constraints must have values
    for (let i = 0; i < this.selectedConstraints.length; i++) {
      const selected = this.selectedConstraints[i];
      if (selected !== undefined && this.constraints[selected] === undefined) {
        return { error: RepeatError.EmptyConstraints };
      }
    }

    if (this.isSelected("start") && this.isSelected("end")) {
      // Must end after start!
      if (this.constraints.end! < this.constraints.start!) {
        return { error: RepeatError.EndBeforeStart };
      }
    }

    if (this.isSelected("duration") && this.constraints.duration! < 0) {
      return { error: RepeatError.NegativeDuration };
    }

    // Must have at least 1 whole number repetition
    if (this.isSelected("repetitions")) {
      if (this.constraints.repetitions! < 1) {
        return { error: RepeatError.TooFewRepetitions };
      }
      if (this.constraints.repetitions! % 1 !== 0) {
        return { error: RepeatError.FractionalRepetitions };
      }
    }

    // Solve for duration (null means infinite)
    let duration: number | null;
    if (this.isSelected("duration")) {
      duration = this.constraints.duration!;
    } else if (this.isSelected("start") && this.isSelected("end")) {
      duration = this.constraints.end! - this.constraints.start!;
    } else if (this.isSelected("interval")) {
      let interval = this.constraints.interval!;
      if (!this.includeChildDuration) {
        interval += childDuration;
      }
      if (interval < childDuration) {
        return { error: RepeatError.IntervalTooShort };
      }
      if (this.isSelected("repetitions")) {
        duration = interval * this.constraints.repetitions!;
        if (!this.trailingInterval) {
          duration -= interval - childDuration;
        }
      } else {
        // Null means duration is infinite
        duration = null;
      }
    } else {
      return { error: RepeatError.InvalidConstraints };
    }

    // Solve for start
    let start: number | null;
    if (this.isSelected("start")) {
      start = this.constraints.start!;
    } else {
      if (duration === null) {
        start = null;
      } else {
        start = this.constraints.end! - duration;
      }
    }

    // Solve for end
    let end: number | null;
    if (this.isSelected("end")) {
      end = this.constraints.end!;
    } else {
      if (duration === null) {
        end = null;
      } else {
        end = this.constraints.start! + duration;
      }
    }

    // Solve for interval
    let interval: number;
    if (this.isSelected("interval")) {
      interval = this.constraints.interval!;
      if (!this.includeChildDuration) {
        interval += childDuration;
      }
      if (interval < childDuration) {
        return { error: RepeatError.IntervalTooShort };
      }
    } else if (duration !== null) {
      const totalGap = duration - childDuration * this.constraints.repetitions!;
      if (totalGap < 0) {
        return { error: RepeatError.DurationTooShort };
      }
      let numGaps = this.constraints.repetitions!;
      if (!this.trailingInterval) {
        numGaps -= 1;
      }
      const gap = totalGap / numGaps;
      interval = childDuration + gap;
    } else {
      return { error: RepeatError.InvalidConstraints };
    }

    // Solve for repetitions
    let repetitions: number;
    if (this.isSelected("repetitions")) {
      repetitions = this.constraints.repetitions!;
    } else if (duration !== null) {
      let fullDuration = duration;
      if (!this.trailingInterval) {
        fullDuration += interval - childDuration;
      }
      repetitions = Math.floor(fullDuration / interval);
      if (repetitions < 1) {
        return { error: RepeatError.DurationTooShort };
      }
    } else {
      return { error: RepeatError.InvalidConstraints };
    }

    // Adjust interval to final value
    if (!this.includeChildDuration) {
      interval -= childDuration;
    }

    return {
      error: RepeatError.None,
      solved: {
        start: start ?? undefined,
        end: end ?? undefined,
        duration: duration ?? undefined,
        repetitions,
        interval,
      },
    };
  }

  toUnitString(
    offset: number | undefined,
    constraint: keyof RepeatConstraints
  ): string {
    if (offset === undefined) return "";
    if (constraint === "repetitions") return offset.toString();

    const pad = (num: number) => `${num < 10 ? "0" : ""}${num}`;
    const serialize = (num: number) => serializeRound(num).toString();

    const unit = this.constraintUnit(constraint);

    if (unit === "seconds" || unit === "unix time") {
      return serialize(offset);
    } else if (unit === "minutes") {
      return serialize(offset / 60);
    } else if (unit === "hours") {
      return serialize(offset / 3600);
    } else if (unit === "HH:MM:SS") {
      const hrs = pad(Math.floor(offset / 3600));
      const min = pad(Math.floor((offset % 3600) / 60));
      const sec = pad(serializeRound(offset) % 60);

      return `${hrs}:${min}:${sec}`;
    } else if (unit === "YYYY-MM-DD HH:MM:SS") {
      const date = new Date(offset * 1000);
      return (
        [
          date.getFullYear(),
          pad(date.getMonth() + 1),
          pad(date.getDate()),
        ].join("-") +
        " " +
        [
          pad(date.getHours()),
          pad(date.getMinutes()),
          pad(date.getSeconds()),
        ].join(":")
      );
    }

    return serialize(offset);
  }

  fromUnitString(
    str: string,
    constraint: keyof RepeatConstraints
  ): number | undefined {
    if (constraint === "repetitions")
      return Number.isNaN(Number(str)) ? undefined : Number(str);
    const unit = this.constraintUnit(constraint);

    let returnVal = undefined;
    if (["seconds", "unix time", "minutes", "hours"].includes(unit)) {
      const num = Number(str);

      if (unit === "seconds" || unit === "unix time") {
        returnVal = num;
      } else if (unit === "minutes") {
        returnVal = num * 60;
      } else if (unit === "hours") {
        returnVal = num * 3600;
      }
    } else if (unit === "HH:MM:SS") {
      let parts = str.split(":");
      let result = 0;
      let multiplier = 1;

      while (parts.length > 0) {
        let num = Number(parts.pop());

        result += num * multiplier;
        multiplier *= 60;
      }

      returnVal = result;
    } else if (unit === "YYYY-MM-DD HH:MM:SS") {
      returnVal = Date.parse(str) / 1000;
    }

    return returnVal === undefined || Number.isNaN(returnVal)
      ? undefined
      : returnVal;
  }

  constraintUnit(constraint: keyof RepeatConstraints) {
    return constraint === "start" || constraint === "end"
      ? this.unit
      : this.durationUnit;
  }

  isUnitNumeric(constraint: keyof RepeatConstraints): boolean {
    if (constraint === "repetitions") return true;
    const unit = this.constraintUnit(constraint);
    return ["seconds", "minutes", "hours", "unix time"].includes(unit);
  }
}
