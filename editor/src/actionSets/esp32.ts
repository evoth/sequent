import {
  EnumParameter,
  NestedParameter,
  NumberParameter,
  Parameter,
} from "../data/parameter";

import { Action } from "../data/action";
import { ActionSet } from "../data/actionSet";

// TODO: add mapped parameter type so enumerated params can still be used as durationParam

export function getEsp32ActionSet(): ActionSet {
  const actionSet = new ActionSet("Test");
  const bulbParam = new NumberParameter<number>(
    actionSet.parameterManager,
    "Bulb exposure",
    "",
    30,
    1,
    undefined,
    undefined,
    "seconds"
  );
  const shutterOptions = [
    "1/4000",
    "1/3200",
    "1/2500",
    "1/2000",
    "1/1600",
    "1/1250",
    "1/1000",
    "1/800",
    "1/640",
    "1/500",
    "1/400",
    "1/320",
    "1/250",
    "1/200",
    "1/160",
    "1/125",
    "1/100",
    "1/80",
    "1/60",
    "1/50",
    "1/40",
    "1/30",
    "1/25",
    "1/20",
    "1/15",
    "1/13",
    "1/10",
    "1/8",
    "1/6",
    "1/5",
    "1/4",
    '0"3',
    '0"4',
    '0"5',
    '0"6',
    '0"8',
    '1"',
    '1"3',
    '1"6',
    '2"',
    '2"5',
    '3"2',
    '4"',
    '5"',
    '6"',
    '8"',
    '10"',
    '13"',
    '15"',
    '20"',
    '25"',
    '30"',
    "bulb",
  ];
  function parseExpSetting(expSetting: string): number {
    if (expSetting === "bulb") return -1;
    let exp = parseInt(expSetting);
    let delim = expSetting.indexOf('"');
    if (delim === -1) {
      delim = expSetting.indexOf("/");
      if (delim !== -1) {
        exp /= parseInt(expSetting.slice(delim + 1));
      }
    } else if (delim < expSetting.length - 1) {
      exp += parseInt(expSetting.slice(delim + 1)) / 10;
    }
    return exp;
  }
  const shutterNested = new Map<number, Parameter<any>[]>(
    shutterOptions.map((shutter) => [parseExpSetting(shutter), []])
  );
  shutterNested.set(-1, [bulbParam]);
  const shutterParam = new NestedParameter<number>(
    actionSet.parameterManager,
    "Shutter speed",
    "",
    0.008,
    shutterNested,
    new Map(
      shutterOptions.map((shutter) => [parseExpSetting(shutter), shutter])
    ),
    new Map(
      shutterOptions.map((shutter) => [parseExpSetting(shutter), shutter])
    )
  );
  const isoParam = new EnumParameter<string>(
    actionSet.parameterManager,
    "ISO",
    "",
    "400",
    [
      "auto",
      "100",
      "125",
      "160",
      "200",
      "250",
      "320",
      "400",
      "500",
      "640",
      "800",
      "1000",
      "1250",
      "1600",
      "2000",
      "2500",
      "3200",
      "4000",
      "5000",
      "6400",
      "8000",
      "10000",
      "12800",
      "16000",
      "20000",
      "25600",
    ]
  );
  const apertureParam = new EnumParameter<string>(
    actionSet.parameterManager,
    "Aperture",
    "",
    "f8.0",
    [
      "f3.5",
      "f4.0",
      "f4.5",
      "f5.0",
      "f5.6",
      "f6.3",
      "f7.1",
      "f8.0",
      "f9.0",
      "f10",
      "f11",
      "f13",
      "f14",
      "f16",
      "f18",
      "f20",
      "f22",
    ]
  );
  const evOptions = [
    "-3.0",
    "-2_2/3",
    "-2_1/3",
    "-2.0",
    "-1_2/3",
    "-1_1/3",
    "-1.0",
    "-0_2/3",
    "-0_1/3",
    "+0.0",
    "+0_1/3",
    "+0_2/3",
    "+1.0",
    "+1_1/3",
    "+1_2/3",
    "+2.0",
    "+2_1/3",
    "+2_2/3",
    "+3.0",
  ];
  const evParam = new EnumParameter<string>(
    actionSet.parameterManager,
    "Exposure compensation",
    "",
    "+0.0",
    evOptions,
    new Map(
      evOptions.map((option) => [
        option,
        option.replace("_", " ")!.replace(".0", "")!.replace("0 ", "")!,
      ])
    )
  );
  const modeParam = new NestedParameter<string>(
    actionSet.parameterManager,
    "Shooting mode",
    "",
    "Aperture priority (Av)",
    new Map(
      Object.entries({
        "Manual (M)": [apertureParam, isoParam, shutterParam],
        "Aperture priority (Av)": [isoParam, apertureParam, evParam],
        "Shutter priority (Tv)": [isoParam, shutterParam, evParam],
        "Program (P)": [isoParam, evParam],
        Auto: [],
      })
    )
  );
  new Action(
    actionSet.actionManager,
    "Photo",
    "Trigger shutter with given settings",
    {
      defaultDuration: 1,
      durationParams: [
        { param: shutterParam, paramOffset: 1 },
        { param: bulbParam, paramOffset: 2 },
      ],
    },
    [modeParam]
  );
  return actionSet;
}
