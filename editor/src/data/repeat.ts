import { Timestamp } from "./timestamp";

export interface Repeatable {
  getDuration(): number | undefined;
}

export class Repeat {
  child: Repeatable;
  props: RepeatProps;
  isRepeating: boolean;

  constructor(child: Repeatable, props: RepeatProps, isRepeating: boolean) {
    this.child = child;
    this.props = props;
    this.isRepeating = isRepeating;
  }
}

export enum RepeatPropsError {
  None,
  WrongNumConstraints,
  InvalidConstraints,
  EmptyConstraints,
  TimestampRootMismatch,
  EndBeforeStart,
  NegativeDuration,
  TooFewRepetitions,
  FractionalRepetitions,
  IntervalTooShort,
  DurationTooShort,
}

export type RepeatConstraints = {
  start?: Timestamp;
  end?: Timestamp;
  duration?: number;
  repetitions?: number;
  interval?: number;
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
export class RepeatProps {
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

  validateConstraints(): RepeatPropsError {
    if (this.selectedConstraints.length !== validConstraints.length + 1) {
      return RepeatPropsError.WrongNumConstraints;
    }
    // Checking validity against the "tree" of valid combinations
    for (let i = 0; i < validConstraints.length; i++) {
      const constraint = this.selectedConstraints[i];
      if (constraint === undefined || !(constraint in validConstraints[i])) {
        return RepeatPropsError.InvalidConstraints;
      }
      // Gets the options that are allowed to follow this one
      const nextConstraintOptions = validConstraints[i][constraint];
      if (nextConstraintOptions === undefined) {
        return RepeatPropsError.InvalidConstraints;
      }
      // Get the following option in the input
      const nextConstraint = this.selectedConstraints[i + 1];
      // Undefined is valid only if the options list is empty
      if (nextConstraint === undefined) {
        if (nextConstraintOptions.length === 0) {
          break;
        } else {
          return RepeatPropsError.InvalidConstraints;
        }
      }
      // If the options list isn't empty, the following option must be in it
      if (!nextConstraintOptions.includes(nextConstraint)) {
        return RepeatPropsError.InvalidConstraints;
      }
    }
    return RepeatPropsError.None;
  }

  isSelected(constraintName: keyof RepeatConstraints | undefined) {
    return this.selectedConstraints.includes(constraintName);
  }

  validate(
    childDuration: number
  ): [error: RepeatPropsError, solved?: RepeatConstraints] {
    const constraintError = this.validateConstraints();
    if (constraintError !== RepeatPropsError.None) {
      return [constraintError];
    }

    // Selected constraints must have values
    for (let i = 0; i < this.selectedConstraints.length; i++) {
      const selected = this.selectedConstraints[i];
      if (selected !== undefined && this.constraints[selected] === undefined) {
        return [RepeatPropsError.EmptyConstraints];
      }
    }

    if (this.isSelected("start") && this.isSelected("end")) {
      const [startOffset, startRoot] = this.constraints.start!.getOffset();
      const [endOffset, endRoot] = this.constraints.end!.getOffset();
      // If both start and end are used, they have to be descended from the same root timestamp
      if (startRoot.id != endRoot.id) {
        return [RepeatPropsError.TimestampRootMismatch];
      }
      // Must end after start!
      if (endOffset < startOffset) {
        return [RepeatPropsError.EndBeforeStart];
      }
    }

    if (this.isSelected("duration") && this.constraints.duration! < 0) {
      return [RepeatPropsError.NegativeDuration];
    }

    // Must have at least 2 whole number repetitions
    if (this.isSelected("repetitions")) {
      if (this.constraints.repetitions! < 2) {
        return [RepeatPropsError.TooFewRepetitions];
      }
      if (this.constraints.repetitions! % 1 !== 0) {
        return [RepeatPropsError.FractionalRepetitions];
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
        return [RepeatPropsError.IntervalTooShort];
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
      return [RepeatPropsError.InvalidConstraints];
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
        return [RepeatPropsError.IntervalTooShort];
      }
    } else if (duration !== null) {
      const totalGap = duration - childDuration * this.constraints.repetitions!;
      if (totalGap < 0) {
        return [RepeatPropsError.DurationTooShort];
      }
      let numGaps = this.constraints.repetitions!;
      if (!this.trailingInterval) {
        numGaps -= 1;
      }
      const gap = totalGap / numGaps;
      interval = childDuration + gap;
    } else {
      return [RepeatPropsError.InvalidConstraints];
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
        return [RepeatPropsError.DurationTooShort];
      }
    } else {
      return [RepeatPropsError.InvalidConstraints];
    }

    // Adjust interval to final value
    if (!this.includeChildDuration) {
      interval -= childDuration;
    }

    return [
      RepeatPropsError.None,
      {
        start: start ?? undefined,
        end: end ?? undefined,
        duration: duration ?? undefined,
        repetitions,
        interval,
      },
    ];
  }
}
