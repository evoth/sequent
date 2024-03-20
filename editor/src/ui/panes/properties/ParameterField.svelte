<script lang="ts">
  import type { ActionState } from "../../../data/action";
  import {
    NestedParameter,
    NumberParameter,
    ParameterError,
    StringParameter,
    type Parameter,
    type ParameterState,
    type ParameterType,
  } from "../../../data/parameter";
  import { updateIndex } from "../../../data/stores";
  import SelectDropdown from "../../utilities/SelectDropdown.svelte";

  export let actionState: ActionState;
  export let parameterState: ParameterState<Parameter<any>, any>;
  export let getParameterState: (
    actionState: ActionState,
    parameter: Parameter<any>
  ) => ParameterState<Parameter<any>, any>;

  let parameter = parameterState.parameter;

  // TODO: check repeat validation as well because parameter can affect duration
  function updateValue<T extends ParameterType>(
    state: ParameterState<Parameter<T>, T>,
    value?: T,
    event?: Event,
    parseFunction?: (value: any) => T
  ) {
    if (value === undefined) {
      if (event === undefined || parseFunction === undefined) return;
      const target = event.target as HTMLInputElement;
      value = parseFunction(
        target.type === "checkbox" ? target.checked : target.value
      );
    }
    const validation = state.parameter.validate(value);
    if (validation.error === ParameterError.None) {
      state.value = value;
      $updateIndex++;
    } else {
      if (event === undefined) return;
      const target = event.target as HTMLInputElement;
      if (validation.fixed) {
        state.value = validation.fixed;
        target.value = String(validation.fixed);
        $updateIndex++;
      } else {
        target.value = String(state.value);
      }
    }
  }
</script>

<div class="container">
  {#if parameter instanceof NestedParameter}
    <SelectDropdown
      label={parameter.name}
      buttonTitle={`Options for ${parameter.name}`}
      dropdownText={parameterState.parameter
        .getDisplayValue(parameterState.value)
        .toString()}
    >
      <svelte:fragment slot="buttons" let:toggleDropdown>
        {#each parameter.nested.keys() as option}
          {#if parameterState.value !== option}
            <button
              on:click={() => {
                updateValue(parameterState, option);
                toggleDropdown();
              }}
            >
              {parameter.getDisplayValue(option)}
            </button>
          {/if}
        {/each}
      </svelte:fragment>
    </SelectDropdown>
  {:else}
    <label class="horizontal">
      <div class="label">{parameter.name}:</div>
      {#if parameter instanceof NumberParameter}
        <input
          type="number"
          step={parameter.step}
          min={parameter.min}
          max={parameter.max}
          value={parameterState.value}
          placeholder={`Value of ${parameter.name}`}
          on:change={(event) =>
            updateValue(parameterState, undefined, event, Number)}
        />
        {parameter.unit ?? ""}
      {:else if parameter instanceof StringParameter}
        <input
          type="text"
          minlength={parameter.minLength}
          maxlength={parameter.maxLength}
          value={parameterState.value}
          placeholder={`Value of ${parameter.name}`}
          on:change={(event) =>
            updateValue(parameterState, undefined, event, String)}
        />
      {:else if typeof parameterState.value === "number"}
        <input
          type="number"
          value={parameterState.value}
          placeholder={`Value of ${parameter.name}`}
          on:change={(event) =>
            updateValue(parameterState, undefined, event, Number)}
        />
      {:else if typeof parameterState.value === "string"}
        <input
          type="text"
          value={parameterState.value}
          placeholder={`Value of ${parameter.name}`}
          on:change={(event) =>
            updateValue(parameterState, undefined, event, String)}
        />
      {:else if typeof parameterState.value === "boolean"}
        <input
          type="checkbox"
          checked={parameterState.value}
          on:change={(event) =>
            updateValue(parameterState, undefined, event, Boolean)}
        />
      {/if}
    </label>
  {/if}
  {#if parameter instanceof NestedParameter && (parameter.nested.get(parameterState.value) ?? []).length > 0}
    <div class="nested">
      {#each parameter.nested.get(parameterState.value) ?? [] as nestedParameter (nestedParameter.id)}
        <svelte:self
          {actionState}
          parameterState={getParameterState(actionState, nestedParameter)}
          {getParameterState}
        />
      {/each}
    </div>
  {/if}
</div>

<style>
  .container {
    display: flex;
    flex-direction: column;
    gap: 0.8rem;
  }

  .nested {
    margin-left: 1rem;
    display: flex;
    flex-direction: column;
    gap: 0.8rem;
  }

  .label {
    flex-shrink: 0;
  }
</style>
