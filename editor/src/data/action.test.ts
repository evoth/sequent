import { NumberParameter, Parameter, ParameterError } from "./parameter";

import { Action } from "./action";

describe("action", () => {
  test("correctly validates parameter", () => {
    let param = new NumberParameter<number>(0, "Test", "Test parameter", 0.5);
    let action = new Action(0, "Test", "Test action", [param]);
    expect(action.parameters[0].validate(0)).toEqual([
      ParameterError.UnderMin,
      0.5,
    ]);
  });
  test("correctly handles required parameters", () => {
    let param = new Parameter<number>(0, "Test", "Test parameter");
    let action = new Action(0, "Test", "Test action", [param]);
    expect(action.isParameterRequired(param)).toBe(false);
    let param2 = new Parameter<number>(1, "Test2", "Test parameter");
    action = new Action(0, "Test", "Test action", [param, param2], [1]);
    expect(action.isParameterRequired(param)).toBe(false);
    expect(action.isParameterRequired(param2)).toBe(true);
  });
});

describe("action state", () => {
  test("correctly stores and validates parameter values", () => {
    let param = new Parameter<number>(0, "Test", "Test parameter");
    let param2 = new NumberParameter<number>(
      1,
      "Test2",
      "Test parameter",
      undefined,
      10
    );
    let action = new Action(0, "Test", "Test action", [param, param2], [1]);
    let actionState = action.newState();
    expect(actionState.parameterStates[0].value).toBe(undefined);
    actionState.parameterStates[0].value = 5;
    expect(actionState.parameterStates[0].value).toBe(5);
    expect(actionState.parameterStates[0].parameter.validate(100)).toEqual([
      ParameterError.None,
    ]);
    expect(actionState.parameterStates[1].parameter.validate(100)).toEqual([
      ParameterError.OverMax,
      10,
    ]);
    let actionState2 = action.newState();
    expect(actionState2.parameterStates[0].value).toBe(undefined);
  });
});
