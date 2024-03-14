<script lang="ts">
  import {
    RepeatError,
    RepeatProps,
    validConstraints,
    type RepeatConstraints,
    type RepeatValidation,
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
    if (target.value === "") {
      target.value = String(prevValue);
      return;
    }
    props.constraints[constraint] = Number(target.value);
    const validationCheck = props.validate(childDuration);
    if (validationCheck.error !== RepeatError.None) {
      props.constraints[constraint] = prevValue;
      target.value = String(prevValue);
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

  function testBoolean(
    key: "trailingInterval" | "includeChildDuration",
    newValue: boolean
  ): RepeatValidation | undefined {
    if (component === undefined || props === undefined) return undefined;
    const childDuration = component.child.getDuration();
    if (childDuration === undefined) return undefined;

    // Tricking Svelte into not realizing I'm updating this object :)))
    const secretPropsRef = props;
    const prevValue = secretPropsRef[key];
    secretPropsRef[key] = newValue;
    const validationCheck = secretPropsRef.validate(childDuration);
    secretPropsRef[key] = prevValue;
    return validationCheck;
  }

  function updateBoolean(
    event: Event,
    key: "trailingInterval" | "includeChildDuration"
  ) {
    if (component === undefined || props === undefined) return undefined;
    const target = event.target as HTMLInputElement;
    const validationCheck = testBoolean(key, target.checked);
    if (validationCheck === undefined || validationCheck.solved === undefined) {
      target.checked = !target.checked;
    } else {
      props[key] = target.checked;
      props.constraints = validationCheck.solved;
    }
    component.props = props;
    $updateIndex++;
  }

  const capitalize = (s: string) => s[0].toUpperCase() + s.substring(1);
</script>

<PaneSection title="Repetition" name={"properties-repitition"}>
  {#if component !== undefined && props !== undefined}
    {#each Object.entries(props.constraints) as [constraint, value]}
      <label class="horizontal">
        {capitalize(constraint)}:
        <input
          type="number"
          step="any"
          min="0"
          {value}
          on:change={(event) => updateConstraint(event, constraint)}
          disabled={!selectedConstraints.includes(constraint)}
          placeholder={constraint === "repetitions"
            ? "Number of repetitions"
            : `${capitalize(constraint)}${constraint === "start" || constraint === "end" ? " time" : ""} in seconds`}
        />
        {constraint !== "repetitions" ? "seconds" : ""}
      </label>
    {/each}
    <div class="boolean-options">
      <label class="horizontal">
        <input
          type="checkbox"
          checked={props.includeChildDuration}
          on:change={(event) => updateBoolean(event, "includeChildDuration")}
          disabled={testBoolean(
            "includeChildDuration",
            !props.includeChildDuration
          )?.error !== RepeatError.None}
        />
        Interval includes child duration
      </label>
      <label class="horizontal">
        <input
          type="checkbox"
          checked={props.trailingInterval}
          on:change={(event) => updateBoolean(event, "trailingInterval")}
          disabled={testBoolean("trailingInterval", !props.includeChildDuration)
            ?.error !== RepeatError.None}
        />
        Include trailing interval
      </label>
    </div>
    {#each { length: 3 } as _, i ([i, selectedConstraints])}
      <!-- TODO: pull this element out into its own component since it's also used almost verbatim in ParameterField -->
      <div class="constraint-dropdown">
        Constraint {i + 1}:
        <Dropdown fullWidth lighter>
          <button
            slot="button"
            let:toggleDropdown
            on:click={toggleDropdown}
            title={`Constraint ${i + 1} options`}
            class="constraint-dropdown-button"
            disabled={getConstraintOptions(i).length <= 1}
          >
            {capitalize(selectedConstraints[i])}
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
                  }}>{capitalize(option)}</button
                >
              {/if}
            {/each}
          </svelte:fragment>
        </Dropdown>
      </div>
    {/each}
  {/if}
</PaneSection>

<style>
  .constraint-dropdown {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }

  .constraint-dropdown-button {
    background-color: var(--gray-85);
    padding: 0.6rem;
    display: flex;
    gap: 0.4rem;
    width: 100%;
    justify-content: space-between;
  }

  .boolean-options {
    display: flex;
    flex-wrap: wrap;
    row-gap: 1rem;
    gap: 1rem;
  }
  .boolean-options > label {
    flex: 1;
    white-space: nowrap;
  }
</style>
