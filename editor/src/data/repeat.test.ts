import { RepeatError, RepeatProps } from "./repeat";

import type { RepeatConstraints } from "./repeat";

test("RepeatProps correctly validates all constraint combinations", () => {
  const options: (keyof RepeatConstraints | undefined)[] = [
    "start",
    "end",
    "duration",
    "repetitions",
    "interval",
    undefined,
  ];
  const valid: (keyof RepeatConstraints | undefined)[][] = [
    ["start", "end", "repetitions"],
    ["start", "end", "interval"],
    ["start", "duration", "repetitions"],
    ["start", "duration", "interval"],
    ["start", "repetitions", "interval"],
    ["start", "interval", undefined],
    ["end", "duration", "repetitions"],
    ["end", "duration", "interval"],
    ["end", "repetitions", "interval"],
    ["end", "interval", undefined],
  ];
  const validOrdinals = valid.map((list) => {
    let ord = 0;
    for (const item of list) {
      ord *= options.length;
      ord += options.indexOf(item);
    }
    return ord;
  });
  for (let i = 0; i < Math.pow(options.length, 3); i++) {
    const isValid = validOrdinals.includes(i);
    const testConstraints: (keyof RepeatConstraints | undefined)[] = [];
    let j = i;
    for (let k = 2; k >= 0; k--) {
      testConstraints[k] = options[j % options.length];
      j = Math.floor(j / options.length);
    }
    const props = new RepeatProps({}, testConstraints);
    expect(props.validateConstraints()).toBe(
      isValid ? RepeatError.None : RepeatError.InvalidConstraints
    );
  }
});
