<script lang="ts">
  import {
    RepeatError,
    RepeatProps,
    validConstraints,
    type RepeatConstraints,
  } from "../../../data/repeat";
  import { Component, Sequence } from "../../../data/sequence";
  import {
    project,
    selectedComponents,
    updateIndex,
  } from "../../../data/stores";
  import Dropdown from "../../utilities/Dropdown.svelte";
  import PaneSection from "../PaneSection.svelte";

  let component: Component | undefined;
  let props: RepeatProps | undefined;
  // TODO: Change back to (keyof RepeatConstraints | undefined)[]
  let selectedConstraints: (keyof RepeatConstraints)[];

  $: component = getComponent($selectedComponents, $project.openedSequence);
  $: selectedConstraints = [
    ...(component?.props.selectedConstraints ?? []),
  ] as (keyof RepeatConstraints)[];
  $: props = getProps(component, selectedConstraints);

  function getComponent(
    selectedComponents: Map<Sequence, Component | undefined>,
    sequence?: Sequence
  ): Component | undefined {
    if (sequence === undefined) return undefined;
    return selectedComponents.get(sequence);
  }

  function getProps(
    newComponent: Component | undefined,
    newSelectedConstraints: (keyof RepeatConstraints | undefined)[]
  ): RepeatProps | undefined {
    if (newComponent === undefined) return;
    const validation = component?.validate();
    if (validation?.solved === undefined) return undefined;
    return new RepeatProps(
      validation.solved,
      newSelectedConstraints,
      newComponent.props.includeChildDuration,
      newComponent.props.trailingInterval
    );
  }

  function updateConstraint(event: Event, constraint: keyof RepeatConstraints) {
    if (component === undefined || props === undefined) return;
    const childDuration = component.child.getDuration();
    if (childDuration === undefined) return;

    const prevValue = props.constraints[constraint];
    const target = event.target as HTMLInputElement;
    props.constraints[constraint] = parseInt(target.value);
    const validationCheck = props.validate(childDuration);
    if (validationCheck.error !== RepeatError.None) {
      props.constraints[constraint] = prevValue;
      target.value = String(prevValue);
      console.log(validationCheck.error);
      return;
    }
    if (validationCheck.solved !== undefined) {
      props.constraints = validationCheck.solved;
    }
    component.props = props;
    $updateIndex++;
  }

  function getConstraintOptions(index: number): (keyof RepeatConstraints)[] {
    let options: (keyof RepeatConstraints)[];
    if (index === 0) {
      options = Object.keys(validConstraints[0]);
    } else {
      options = validConstraints[index - 1][selectedConstraints[index - 1]]!;
    }
    return options;
  }

  function selectConstraint(
    newConstraint: keyof RepeatConstraints,
    index: number
  ) {
    selectedConstraints[index] = newConstraint;
    for (const [i, constraint] of selectedConstraints.entries()) {
      const options = getConstraintOptions(i);
      if (!options.includes(constraint)) {
        selectedConstraints[i] = options[0];
      }
    }
  }
</script>

<PaneSection title="Repetition" name={"properties-repitition"}>
  {#if component !== undefined && props !== undefined}
    {#each { length: 3 } as _, i ([i, selectedConstraints])}
      <div class="constraint-menu">
        <div>Constraint {i + 1}:</div>
        <Dropdown fullWidth lighter>
          <button
            slot="button"
            let:toggleDropdown
            on:click={toggleDropdown}
            title={`Constraint ${i + 1} options`}
            class="constraint-button"
            disabled={getConstraintOptions(i).length <= 1}
          >
            {selectedConstraints[i]}
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
            {#each getConstraintOptions(i) as option ([option, selectedConstraints])}
              {#if selectedConstraints[i] !== option}
                <button
                  on:click={() => {
                    selectConstraint(option, i);
                    toggleDropdown();
                  }}>{option}</button
                >
              {/if}
            {/each}
          </svelte:fragment>
        </Dropdown>
      </div>
    {/each}
    {#each Object.entries(props.constraints) as [constraint, value]}
      <label class="horizontal">
        {constraint[0].toUpperCase() + constraint.substring(1)}:
        <input
          type="number"
          step="any"
          min="0"
          {value}
          on:change={(event) => updateConstraint(event, constraint)}
          disabled={!selectedConstraints.includes(constraint)}
        />
      </label>
    {/each}
  {/if}
</PaneSection>

<style>
  .constraint-menu {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }

  .constraint-button {
    background-color: var(--gray-85);
    padding: 0.6rem;
    display: flex;
    gap: 0.4rem;
    width: 100%;
    justify-content: space-between;
  }
</style>
