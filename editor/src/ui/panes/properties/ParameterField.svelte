<script lang="ts">
  import type { ActionState } from "../../../data/action";
  import {
    EnumParameter,
    NestedParameter,
    NumberParameter,
    ParameterError,
    StringParameter,
    type Parameter,
    type ParameterState,
    type ParameterType,
  } from "../../../data/parameter";
  import { updateIndex } from "../../../data/stores";
  import Dropdown from "../../utilities/Dropdown.svelte";

  export let actionState: ActionState;
  export let parameterState: ParameterState<Parameter<any>, any>;
  export let getParameterState: (
    actionState: ActionState,
    parameter: Parameter<any>
  ) => ParameterState<Parameter<any>, any>;

  let parameter = parameterState.parameter;

  function getNested<T extends ParameterType>(
    parameter: NestedParameter<T>,
    value: T
  ) {
    return parameter.nested.get(value) ?? [];
  }

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
  {#if parameter instanceof EnumParameter || parameter instanceof NestedParameter}
    <div class="parameter-dropdown">
      {parameter.name}:
      <Dropdown fullWidth lighter>
        <button
          slot="button"
          let:toggleDropdown
          on:click={toggleDropdown}
          title={`Options for ${parameter.name}`}
          class="parameter-dropdown-button"
        >
          {parameterState.value}
          <svg
            xmlns="http://www.w3.org/2000/svg"
            width="1.3em"
            height="1.3em"
            viewBox="0 0 24 24"
            fill="none"
            stroke="currentColor"
            stroke-width="2"
            stroke-linecap="round"
            stroke-linejoin="round"><path d="M6 9l6 6 6-6" /></svg
          >
        </button>
        <svelte:fragment slot="buttons" let:toggleDropdown>
          {#if parameter instanceof EnumParameter && parameter.options.length > 1}
            {#each parameter.options as option}
              {#if parameterState.value !== option}
                <button
                  on:click={() => {
                    updateValue(parameterState, option);
                    toggleDropdown();
                  }}
                >
                  {option}
                </button>
              {/if}
            {/each}
          {:else if parameter instanceof NestedParameter && parameter.nested.size > 1}
            {#each parameter.nested.keys() as option}
              {#if parameterState.value !== option}
                <button
                  on:click={() => {
                    updateValue(parameterState, option);
                    toggleDropdown();
                  }}
                >
                  {option}
                </button>
              {/if}
            {/each}
          {/if}
        </svelte:fragment>
      </Dropdown>
    </div>
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
  <div class="nested">
    {#if parameter instanceof NestedParameter}
      {#each parameter.nested.get(parameterState.value) ?? [] as nestedParameter (nestedParameter.id)}
        <svelte:self
          {actionState}
          parameterState={getParameterState(actionState, nestedParameter)}
          {getParameterState}
        />
      {/each}
    {/if}
  </div>
</div>

<style>
  .container {
    display: flex;
    flex-direction: column;
    gap: 0.6rem;
  }

  .nested {
    margin-left: 1rem;
  }

  .label {
    flex-shrink: 0;
  }
  .parameter-dropdown {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }

  .parameter-dropdown-button {
    background-color: var(--gray-85);
    padding: 0.6rem;
    display: flex;
    gap: 0.4rem;
    width: 100%;
    justify-content: space-between;
  }
</style>
