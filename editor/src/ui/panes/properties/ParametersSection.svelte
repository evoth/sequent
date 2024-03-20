<script lang="ts">
  import { ActionState } from "../../../data/action";
  import type { Parameter, ParameterState } from "../../../data/parameter";
  import { Component, Sequence } from "../../../data/sequence";
  import { project, selectedComponents } from "../../../data/stores";
  import PaneSection from "../PaneSection.svelte";
  import ParameterField from "./ParameterField.svelte";

  let component: Component | undefined;

  $: component = getComponent($selectedComponents, $project.openedSequence);

  function getComponent(
    selectedComponents: Map<Sequence, Component | undefined>,
    sequence?: Sequence
  ): Component | undefined {
    if (sequence === undefined) return undefined;
    return selectedComponents.get(sequence);
  }

  function getParameterState(
    actionState: ActionState,
    parameter: Parameter<any>
  ): ParameterState<Parameter<any>, any> {
    const state = actionState.parameterStates.get(parameter.id);
    if (state === undefined)
      throw new Error(
        `ParameterState does not exist for Parameter with ID ${parameter.id}`
      );
    return state;
  }
</script>

<PaneSection title="Parameters" name={"properties-parameters"}>
  {#if component !== undefined}
    {#if component.child instanceof Sequence}
      Sequence has no parameters.
    {:else if component.child instanceof ActionState}
      {#if component.child.action.parameters.length === 0}
        Action has no parameters.
      {:else}
        {#each component.child.action.parameters as parameter}
          <ParameterField
            actionState={component.child}
            parameterState={getParameterState(component.child, parameter)}
            {getParameterState}
          />
        {/each}
      {/if}
    {/if}
  {/if}
</PaneSection>
