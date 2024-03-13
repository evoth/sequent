<script lang="ts">
  import {
    RepeatError,
    type RepeatConstraints,
    type RepeatValidation,
    type SolvedRepeatConstraints,
  } from "../../../data/repeat";
  import { Component, Sequence } from "../../../data/sequence";
  import {
    project,
    selectedComponents,
    updateIndex,
  } from "../../../data/stores";
  import PaneSection from "../PaneSection.svelte";

  let component: Component | undefined;
  let validation: RepeatValidation | undefined;
  let constraints: SolvedRepeatConstraints | undefined;
  let selectedConstraints: (keyof RepeatConstraints | undefined)[];

  $: component = getComponent($selectedComponents, $project.openedSequence);
  $: validation = component?.validate();
  $: selectedConstraints = component?.props.selectedConstraints ?? [];

  function getComponent(
    selectedComponents: Map<Sequence, Component | undefined>,
    sequence?: Sequence
  ): Component | undefined {
    if (sequence === undefined) return undefined;
    return selectedComponents.get(sequence);
  }
</script>

<PaneSection title="Repetition" name={"properties-repitition"}>
  {#if component !== undefined && validation !== undefined}
    {#if validation.error !== RepeatError.None}
      <p>RepeatError: {validation.error}</p>
    {:else if validation.solved !== undefined}
      {#each Object.entries(validation.solved) as [constraint, value]}
        {#if selectedConstraints.includes(constraint)}
          <label class="horizontal">
            {constraint[0].toUpperCase() + constraint.substring(1)}:
            <input
              type="number"
              step="any"
              min="0"
              bind:value={component.props.constraints[constraint]}
              on:change={() => $updateIndex++}
            />
          </label>
        {:else}
          <p>
            {constraint[0].toUpperCase() + constraint.substring(1)}: {value}
          </p>
        {/if}
      {/each}
    {/if}
  {/if}
</PaneSection>
