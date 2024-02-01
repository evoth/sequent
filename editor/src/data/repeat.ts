import { Action } from "./action";
import { Sequence } from "./sequence";
import type { Timestamp } from "./timestamp";

export interface Repeatable {
  getDuration(): number | undefined;
}

export class Repeat {
  id: number;
  child: Repeatable;
  isRepeating: boolean;
}

export enum RepeatPropsError {
  None,
  NoEndpoint,
  NoRepsOrInterval,
  Overconstrained,
  OnlyRepsAndEndpoint,
  MismatchedBaseTimestamp,
  EndBeforeStart,
  NegativeDuration,
  TooFewRepetitions,
  FractionalRepetitions,
}

export class RepeatProps {
  start?: Timestamp;
  end?: Timestamp;
  duration?: number;
  repetitions?: number;
  interval?: number;
  includeChildDuration: boolean;
  trailingInterval: boolean;

  constructor(
    start?: Timestamp,
    end?: Timestamp,
    duration?: number,
    repetitions?: number,
    interval?: number,
    includeChildDuration: boolean = false,
    trailingInterval: boolean = false
  ) {
    this.start = start;
    this.end = end;
    this.duration = duration;
    this.repetitions = repetitions;
    this.interval = interval;
    this.includeChildDuration = includeChildDuration;
    this.trailingInterval = trailingInterval;
  }

  validateConstraints(): RepeatPropsError {
    if (this.start === undefined && this.end === undefined) {
      // Have to have either start or end
      return RepeatPropsError.NoEndpoint;
    }
    if (this.repetitions === undefined && this.interval === undefined) {
      // Have to have either repetitions or interval
      return RepeatPropsError.NoRepsOrInterval;
    }
    if (this.start !== undefined && this.end !== undefined) {
      if (this.duration !== undefined) {
        // Can't have duration + start & end
        return RepeatPropsError.Overconstrained;
      }
      if (this.repetitions !== undefined && this.interval !== undefined) {
        // Can't have start + end + repetitions + interval
        return RepeatPropsError.Overconstrained;
      }
    } else {
      if (this.duration !== undefined) {
        if (this.repetitions !== undefined && this.interval !== undefined) {
          // Can't have duration + repetitions + interval
          return RepeatPropsError.Overconstrained;
        }
      } else if (this.repetitions !== undefined) {
        // Can't only have repetitions + start/end
        return RepeatPropsError.OnlyRepsAndEndpoint;
      }
    }
    return RepeatPropsError.None;
  }

  validateTimes(): RepeatPropsError {
    if (this.start !== undefined && this.end !== undefined) {
      const [startOffset, startAbsolute] = this.start.getOffset();
      const [endOffset, endAbsolute] = this.end.getOffset();
      if (startAbsolute.id != endAbsolute.id) {
        return RepeatPropsError.MismatchedBaseTimestamp;
      }
      if (endOffset < startOffset) {
        return RepeatPropsError.EndBeforeStart;
      }
    }
    if (this.duration !== undefined && this.duration < 0) {
      return RepeatPropsError.NegativeDuration;
    }
  }

  validate(): RepeatPropsError {
    const constraintError = this.validateHelper();
    if (constraintError) {
      return constraintError;
    }
    if (this.repetitions !== undefined) {
      if (this.repetitions < 2) {
        return RepeatPropsError.TooFewRepetitions;
      }
      if (this.repetitions % 1 !== 0) {
        return RepeatPropsError.FractionalRepetitions;
      }
    }
  }

  stuff(childDuration: number) {
    let interval: number;
    let duration: number;
    if (this.duration !== undefined) {
      duration = this.duration;
    } else if (this.start !== undefined && this.end !== undefined) {
      duration = this.start;
    }
    if (this.interval !== undefined) {
      interval = this.interval;
      if (!this.includeChildDuration) {
        interval += childDuration;
      }
    }
  }
}
