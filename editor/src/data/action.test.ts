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
      1,
      0.5
    );
    let action = new Action(
      actionManager,
      "Test",
      "Test action",
      {
        defaultDuration: 0,
      },
      [param]
    );
    expect(action.parameters[0].validate(0)).toEqual({
      error: ParameterError.UnderMin,
      fixed: 0.5,
    });
  });
});

describe("action state", () => {
  test("correctly stores and validates parameter values", () => {
    let paramManager = new Manager<Parameter<any>>();
    let actionManager = new Manager<Action>();
    let param = new Parameter<number>(
      paramManager,
      "Test",
      "Test parameter",
      17
    );
    let paramId = param.id;
    let param2 = new NumberParameter<number>(
      paramManager,
      "Test2",
      "Test parameter",
      0,
      undefined,
      10
    );
    let param2Id = param2.id;
    let action = new Action(
      actionManager,
      "Test",
      "Test action",
      { defaultDuration: 0 },
      [param, param2]
    );
    let actionState = action.newState();
    expect(actionState.parameterStates.get(paramId)!.value).toBe(17);
    actionState.parameterStates.get(paramId)!.value = 5;
    expect(actionState.parameterStates.get(paramId)!.value).toBe(5);
    expect(
      actionState.parameterStates.get(paramId)!.parameter.validate(100)
    ).toEqual({ error: ParameterError.None });
    expect(
      actionState.parameterStates.get(param2Id)!.parameter.validate(100)
    ).toEqual({ error: ParameterError.OverMax, fixed: 10 });
    let actionState2 = action.newState();
    expect(actionState2.parameterStates.get(paramId)!.value).toBe(17);
  });
});
