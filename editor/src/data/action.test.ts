import { NumberParameter, Parameter, ParameterError } from "./parameter";

import { Action } from "./action";
import { Manager } from "./manager";

describe("action", () => {
  test("correctly validates parameter", () => {
    let paramManager = new Manager<Parameter<any>>();
    let actionManager = new Manager<Action>();
    let param = new NumberParameter<number>(
      paramManager,
      "Test",
      "Test parameter",
      0.5
    );
    let action = new Action(actionManager, "Test", "Test action", [param]);
    expect(action.parameters[0].validate(0)).toEqual([
      ParameterError.UnderMin,
      0.5,
    ]);
  });
});

describe("action state", () => {
  test("correctly stores and validates parameter values", () => {
    let paramManager = new Manager<Parameter<any>>();
    let actionManager = new Manager<Action>();
    let param = new Parameter<number>(paramManager, "Test", "Test parameter");
    let param2 = new NumberParameter<number>(
      paramManager,
      "Test2",
      "Test parameter",
      undefined,
      10
    );
    let action = new Action(actionManager, "Test", "Test action", [
      param,
      param2,
    ]);
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
