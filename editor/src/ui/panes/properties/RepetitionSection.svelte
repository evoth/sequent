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
  import SelectDropdown from "../../utilities/SelectDropdown.svelte";
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
      newComponent.props.trailingInterval,
      newComponent.props.keepLeading
    );
  }

  function updateConstraint(event: Event, constraint: keyof RepeatConstraints) {
    if (component === undefined || props === undefined) return;
    const childDuration = component.child.getDuration(props.keepLeading);
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
    key: "trailingInterval" | "includeChildDuration" | "keepLeading",
    newValue: boolean
  ): RepeatValidation | undefined {
    if (component === undefined || props === undefined) return undefined;
    const childDuration = component.child.getDuration(false);
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
    key: "trailingInterval" | "includeChildDuration" | "keepLeading"
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
      {#if component.child instanceof Sequence}
        <label class="horizontal">
          <input
            type="checkbox"
            checked={props.keepLeading}
            on:change={(event) => updateBoolean(event, "keepLeading")}
            disabled={testBoolean("keepLeading", !props.keepLeading)?.error !==
              RepeatError.None}
          />
          Keep leading space
        </label>
      {/if}
    </div>
    {#each { length: 3 } as _, i ([i, selectedConstraints])}
      <SelectDropdown
        label={`Constraint ${i + 1}`}
        buttonTitle={`Constraint ${i + 1} options`}
        dropdownText={capitalize(selectedConstraints[i])}
        disabled={getConstraintOptions(i).length <= 1}
      >
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
      </SelectDropdown>
    {/each}
  {/if}
</PaneSection>

<style>
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
