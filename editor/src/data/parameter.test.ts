import {
  EnumParameter,
  NumberParameter,
  Parameter,
  ParameterError,
  ParameterState,
  StringParameter,
} from "./parameter";

import { Manager } from "./manager";

describe("generic parameter allows creation and validation for", () => {
  test("numbers", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new Parameter<number>(manager, "Test", "Test parameter", 0);
    expect(param.validate(0)).toEqual([ParameterError.None]);
  });
  test("strings", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new Parameter<string>(manager, "Test", "Test parameter", "");
    expect(param.validate("")).toEqual([ParameterError.None]);
    expect(param.validate("abc")).toEqual([ParameterError.None]);
  });
  test("booleans", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new Parameter<boolean>(
      manager,
      "Test",
      "Test parameter",
      false
    );
    expect(param.validate(false)).toEqual([ParameterError.None]);
    expect(param.validate(true)).toEqual([ParameterError.None]);
  });
});

describe("number parameter correctly validates", () => {
  test("with no bounds", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new NumberParameter<number>(
      manager,
      "Test",
      "Test parameter",
      0
    );
    expect(param.validate(10)).toEqual([ParameterError.None]);
    expect(param.validate(-500.1)).toEqual([ParameterError.None]);
  });
  test("with lower and upper bounds", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new NumberParameter<number>(
      manager,
      "Test",
      "Test parameter",
      0,
      -0.5,
      5
    );
    expect(param.validate(-1)).toEqual([ParameterError.UnderMin, -0.5]);
    expect(param.validate(-0.5)).toEqual([ParameterError.None]);
    expect(param.validate(5)).toEqual([ParameterError.None]);
    expect(param.validate(5.1)).toEqual([ParameterError.OverMax, 5]);
  });
  test("with bounds and step constraint", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new NumberParameter<number>(
      manager,
      "Test",
      "Test parameter",
      0,
      -0.5,
      5,
      0.1
    );
    expect(param.validate(-1.05)).toEqual([ParameterError.UnderMin, -0.5]);
    // Floating point precision
    let validation = param.validate(-0.35);
    expect(validation.error).toBe(ParameterError.WrongStep);
    expect(validation.fixed).toBeCloseTo(-0.3, 9);
    validation = param.validate(4.95);
    expect(validation.error).toBe(ParameterError.WrongStep);
    expect(validation.fixed).toBeCloseTo(5, 9);
    // Fixed value conforms to min/max when rounding exceeds them
    param = new NumberParameter<number>(
      manager,
      "Test",
      "Test parameter",
      7,
      1.2,
      7,
      10
    );
    expect(param.validate(2.4)).toEqual([ParameterError.WrongStep, 1.2]);
    expect(param.validate(5)).toEqual([ParameterError.WrongStep, 7]);
  });
});

describe("string parameter correctly validates", () => {
  test("with no bounds", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new StringParameter<string>(
      manager,
      "Test",
      "Test parameter",
      ""
    );
    expect(param.validate("")).toEqual([ParameterError.None]);
    expect(param.validate("abcdefghijklmnopqrstuvwxyz")).toEqual([
      ParameterError.None,
    ]);
  });
  test("with lower bound", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new StringParameter<string>(
      manager,
      "Test",
      "Test parameter",
      "abc",
      1
    );
    expect(param.validate("")).toEqual([ParameterError.UnderMinLength]);
    expect(param.validate("a")).toEqual([ParameterError.None]);
  });
  test("with lower and upper bounds", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new StringParameter<string>(
      manager,
      "Test",
      "Test parameter",
      "abc",
      2,
      5
    );
    expect(param.validate("a")).toEqual([ParameterError.UnderMinLength]);
    expect(param.validate("ab")).toEqual([ParameterError.None]);
    expect(param.validate("abcde")).toEqual([ParameterError.None]);
    expect(param.validate("abcdef")).toEqual([
      ParameterError.OverMaxLength,
      "abcde",
    ]);
  });
});

describe("enum parameter correctly validates", () => {
  test("number enums", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new EnumParameter<number>(
      manager,
      "Test",
      "Test parameter",
      -1,
      [-1, 3]
    );
    expect(param.validate(0)).toEqual([ParameterError.BadEnumOption]);
    expect(param.validate(-3)).toEqual([ParameterError.BadEnumOption]);
    expect(param.validate(-1)).toEqual([ParameterError.None]);
    expect(param.validate(3)).toEqual([ParameterError.None]);
  });
  test("string enums", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new EnumParameter<string>(
      manager,
      "Test",
      "Test parameter",
      "",
      ["", "abc"]
    );
    expect(param.validate("ab")).toEqual([ParameterError.BadEnumOption]);
    expect(param.validate("abc ")).toEqual([ParameterError.BadEnumOption]);
    expect(param.validate(" ")).toEqual([ParameterError.BadEnumOption]);
    expect(param.validate("")).toEqual([ParameterError.None]);
    expect(param.validate("abc")).toEqual([ParameterError.None]);
  });
});

describe("parameter state", () => {
  test("stores value and validates correctly", () => {
    let manager = new Manager<Parameter<any>>();
    let param = new NumberParameter<number>(
      manager,
      "Test",
      "Test parameter",
      2,
      2,
      10,
      10
    );
    let paramState = new ParameterState<NumberParameter<number>, number>(param);
    expect(paramState.value).toBe(2);
    expect(param.validate(5)).toEqual([ParameterError.WrongStep, 10]);
    expect(param.validate(4.9)).toEqual([ParameterError.WrongStep, 2]);
    expect(param.validate(10)).toEqual([ParameterError.None]);
    paramState.value = 2;
    expect(paramState.value).toBe(2);
    paramState = param.newState(5);
    expect(paramState.value).toBe(5);
    expect(param.validate(5)).toEqual([ParameterError.WrongStep, 10]);
  });
});
