import { ActionState } from "./action";
import type { IdType } from "./manager";
import { Sequence } from "./sequence";
import { Timestamp } from "./timestamp";

export interface Repeatable {
  // An undefined duration means infinite
  getDuration(): number | undefined;
}

export class Repeat implements Serializable {
  child: Repeatable;
  props: RepeatProps;
  isRepeating: boolean;
  // If sequence passes in root timestamp, all timestamps must descend from it.
  // Otherwise (as in the case of the root sequence), any timestamp ancestor
  // is fine as long as start/end timestamps are consistent with each other.
  rootTimestamp?: Timestamp;

  constructor(
    child: Repeatable,
    props: RepeatProps,
    isRepeating: boolean,
    rootTimestamp?: Timestamp
  ) {
    this.child = child;
    this.props = props;
    this.isRepeating = isRepeating;
    this.rootTimestamp = rootTimestamp;
  }

  repeatJSON(): CustomJSON<Repeat> {
    let childJSON: { type: string; json?: Serializable; id?: IdType };
    if (this.child instanceof ActionState) {
      childJSON = { type: "Action", json: this.child };
    } else if (this.child instanceof Sequence) {
      childJSON = { type: "Sequence", id: this.child.id };
    } else {
      childJSON = { type: "Unknown" };
    }
    return {
      child: childJSON,
      props: this.props,
      isRepeating: this.isRepeating,
      rootTimestamp: this.rootTimestamp?.id,
    };
  }

  toJSON(): CustomJSON<Repeat> {
    return this.repeatJSON();
  }

  validate(): RepeatValidation {
    if (!this.isRepeating) return { error: RepeatError.None };

    const childDuration = this.child.getDuration();
    if (childDuration === undefined)
      return { error: RepeatError.ChildDurationInfinite };
    return this.props.validate(childDuration);
  }

  getDuration(): [error: RepeatError, duration?: number] {
    if (!this.isRepeating) return [RepeatError.None, this.child.getDuration()];

    const childDuration = this.child.getDuration();
    if (childDuration === undefined) return [RepeatError.ChildDurationInfinite];

    const { error, solved } = this.props.validate(
      childDuration,
      this.rootTimestamp
    );
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
  TimestampRootMismatch,
  WrongRootTimestamp,
  EndBeforeStart,
  NegativeDuration,
  TooFewRepetitions,
  FractionalRepetitions,
  IntervalTooShort,
  DurationTooShort,
  ChildDurationInfinite,
}

export type RepeatConstraints = {
  start?: Timestamp;
  end?: Timestamp;
  duration?: number;
  repetitions?: number;
  interval?: number;
};

export type SolvedRepeatConstraints = {
  start?: Timestamp;
  end?: Timestamp;
  duration?: number;
  repetitions: number;
  interval: number;
};

// Represents a tree of valid constraints. For each step, the keys are the
// valid values and the values are lists of valid constraints that can follow.
export const validConstraints: {
  [key in keyof RepeatConstraints]?: (keyof RepeatConstraints)[];
}[] = [
  {
    start: ["end", "duration", "repetitions", "interval"],
    end: ["duration", "repetitions", "interval"],
  },
  {
    end: ["repetitions", "interval"],
    duration: ["repetitions", "interval"],
    repetitions: ["interval"],
    interval: [],
  },
];

// Describes how a repeatable object is to be repeated and provides in-depth
// validations of properties
export class RepeatProps implements Serializable {
  constraints: RepeatConstraints;
  selectedConstraints: (keyof RepeatConstraints | undefined)[];
  includeChildDuration: boolean;
  trailingInterval: boolean;

  constructor(
    constraints: RepeatConstraints,
    selectedConstraints: (keyof RepeatConstraints | undefined)[],
    includeChildDuration: boolean = false,
    trailingInterval: boolean = false
  ) {
    this.constraints = constraints;
    this.selectedConstraints = selectedConstraints;
    this.includeChildDuration = includeChildDuration;
    this.trailingInterval = trailingInterval;
  }

  toJSON(): CustomJSON<RepeatProps> {
    return {
      constraints: {
        start: this.constraints.start?.id,
        end: this.constraints.end?.id,
        duration: this.constraints.duration,
        repetitions: this.constraints.repetitions,
        interval: this.constraints.interval,
      },
      selectedConstraints: this.selectedConstraints,
      includeChildDuration: this.includeChildDuration,
      trailingInterval: this.trailingInterval,
    };
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

  validate(childDuration: number, rootTimestamp?: Timestamp): RepeatValidation {
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
      const [startOffset, startRoot] = this.constraints.start!.getOffset();
      const [endOffset, endRoot] = this.constraints.end!.getOffset();
      // If both start and end are used, they have to be descended from the same root timestamp
      if (startRoot !== endRoot) {
        return { error: RepeatError.TimestampRootMismatch };
      }
      // Must end after start!
      if (endOffset < startOffset) {
        return { error: RepeatError.EndBeforeStart };
      }
    }

    // If rootTimestamp is supplied, both timestamps must be descended from it
    if (rootTimestamp !== undefined) {
      if (
        this.isSelected("start") &&
        this.constraints.start!.getOffset()[1] !== rootTimestamp
      ) {
        return { error: RepeatError.WrongRootTimestamp };
      }
      if (
        this.isSelected("end") &&
        this.constraints.end!.getOffset()[1] !== rootTimestamp
      ) {
        return { error: RepeatError.WrongRootTimestamp };
      }
    }

    if (this.isSelected("duration") && this.constraints.duration! < 0) {
      return { error: RepeatError.NegativeDuration };
    }

    // Must have at least 2 whole number repetitions
    if (this.isSelected("repetitions")) {
      if (this.constraints.repetitions! < 2) {
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
      duration =
        this.constraints.end!.getOffset()[0] -
        this.constraints.start!.getOffset()[0];
    } else if (this.isSelected("interval")) {
      let interval = this.constraints.interval!;
      if (!this.includeChildDuration) {
        interval += childDuration;
      } else if (interval < childDuration) {
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
    let start: Timestamp | null;
    if (this.isSelected("start")) {
      start = this.constraints.start!;
    } else {
      if (duration === null) {
        start = null;
      } else {
        const end = this.constraints.end!;
        start = new Timestamp(
          end.manager,
          end.value - duration,
          end.relativeTo
        );
      }
    }

    // Solve for end
    let end: Timestamp | null;
    if (this.isSelected("end")) {
      end = this.constraints.end!;
    } else {
      if (duration === null) {
        end = null;
      } else {
        const start = this.constraints.start!;
        end = new Timestamp(
          start.manager,
          start.value + duration,
          start.relativeTo
        );
      }
    }

    // Solve for interval
    let interval: number;
    if (this.isSelected("interval")) {
      interval = this.constraints.interval!;
      if (!this.includeChildDuration) {
        interval += childDuration;
      } else if (interval < childDuration) {
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
      if (repetitions < 2) {
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
}
