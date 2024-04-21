<script lang="ts">
  import { sprintf } from "sprintf-js";
  import { ActionState } from "../../data/action";
  import { NumberParameter, Parameter } from "../../data/parameter";
  import { Component, LayerMode } from "../../data/sequence";

  export let component: Component;
  export let disabled = false;
  export let shadow = false;
  export let highlight = false;

  let child = component.child.getManageableChild();
  let isRepeating = (component.validate().solved?.repetitions ?? 0) > 1;

  function getDisplayParamValueString(
    displayParam: Parameter<any>,
    value: any,
    name?: string,
    valueTemplate?: string
  ) {
    const displayValue = displayParam.getDisplayValue(value);
    if (valueTemplate === undefined) {
      let valueString = String(displayValue);
      if (
        displayParam instanceof NumberParameter &&
        displayParam.unit !== undefined
      ) {
        valueString += " " + displayParam.unit;
      }
      return valueString;
    } else {
      return sprintf(valueTemplate, displayValue);
    }
  }
</script>

{#if component.layerMode === LayerMode.Override && !shadow}
  <div class="override-shadow"></div>
{/if}
<div
  class="container"
  class:disabled
  class:shadow
  class:highlight
  style:background-color={`hsl(${child.hue}deg var(--component-saturation) var(--component-lightness))`}
>
  <div class="spacer"></div>
  <div class="column">
    <div class="label">
      {child.name}
      {#if isRepeating}
        <svg
          xmlns="http://www.w3.org/2000/svg"
          width="1em"
          height="1em"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          stroke-width="2"
          stroke-linecap="round"
          stroke-linejoin="round"
          ><path
            d="M21.5 2v6h-6M2.5 22v-6h6M2 11.5a10 10 0 0 1 18.8-4.3M22 12.5a10 10 0 0 1-18.8 4.2"
          /></svg
        >
      {/if}
    </div>
    {#if component.child instanceof ActionState}
      {#each component.child.action.displayParams as [displayParam, name, valueTemplate]}
        {#if component.child.getActiveParameters().has(displayParam)}
          <p class="parameter">
            {#if name !== ""}
              {name ?? displayParam.name}:
            {/if}
            <span class="parameter-value"
              >{getDisplayParamValueString(
                displayParam,
                component.child.parameterStates.get(displayParam.id)?.value,
                name,
                valueTemplate
              )}</span
            >
          </p>
        {/if}
      {/each}
    {/if}
  </div>
  <div class="spacer"></div>
</div>

<style>
  .container {
    position: absolute;
    left: 1px;
    right: 1px;
    border-radius: 1rem;
    padding: 1rem 0.2rem;
    cursor: pointer;
    bottom: 5%;
    top: 5%;
    overflow: hidden;
    display: flex;
    align-items: start;
  }

  .override-shadow {
    position: absolute;
    content: "";
    left: 0;
    right: 0;
    top: 50%;
    height: 100vh;
    background-color: color-mix(in srgb, var(--black) 15%, transparent);
    pointer-events: none;
  }

  .disabled {
    cursor: not-allowed;
    opacity: 50%;
  }

  .shadow {
    box-shadow:
      0 20px 25px -5px var(--shadow),
      0 8px 10px -6px var(--shadow);
  }

  .highlight {
    border: 2px solid var(--gray-35);
  }
  .highlight::after {
    content: "";
    position: absolute;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
    border-radius: 1rem;
    background-color: var(--gray-35);
    opacity: 15%;
  }

  .spacer {
    width: 0.8rem;
  }

  .label {
    display: flex;
    align-items: center;
    gap: 0.4rem;
    font-weight: 500;
    margin-bottom: 0.3rem;
    white-space: nowrap;
  }

  .column {
    display: flex;
    flex-direction: column;
  }

  .parameter {
    white-space: nowrap;
    color: var(--gray-10);
  }

  .parameter-value {
    color: var(--gray-25);
  }
</style>
