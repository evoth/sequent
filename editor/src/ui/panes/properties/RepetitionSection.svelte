<script lang="ts">
  import {
    RepeatError,
    RepeatProps,
    validConstraints,
    type RepeatConstraints,
    type RepeatRelativeUnitType,
    type RepeatUnitType,
    type RepeatValidation,
  } from "../../../data/repeat";
  import { Component, LayerMode, Sequence } from "../../../data/sequence";
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
      newComponent.props.keepLeading,
      newComponent.props.unit,
      newComponent.props.durationUnit
    );
  }

  function updateConstraint(event: Event, constraint: keyof RepeatConstraints) {
    if (component === undefined || props === undefined) return;
    const childDuration = component.child.getDuration(props.keepLeading);
    if (childDuration === undefined) return;

    const prevValue = props.constraints[constraint];
    const target = event.target as HTMLInputElement;
    const convertedValue = props.fromUnitString(target.value, constraint);
    if (target.value === "" || convertedValue === undefined) {
      target.value = props.toUnitString(prevValue, constraint);
      return;
    }
    props.constraints[constraint] = convertedValue;
    const validationCheck = props.validate(childDuration);
    if (validationCheck.error !== RepeatError.None) {
      props.constraints[constraint] = prevValue;
      target.value = props.toUnitString(prevValue, constraint);
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

  function getUnitOptions(): RepeatUnitType[] {
    if ($project.openedSequence === undefined) return [];
    return $project.openedSequence.isAbsolute
      ? ["YYYY-MM-DD HH:MM:SS", "unix time"]
      : ["seconds", "minutes", "hours", "HH:MM:SS"];
  }

  function getDurationUnitOptions(): RepeatRelativeUnitType[] {
    return ["seconds", "minutes", "hours", "HH:MM:SS"];
  }

  function selectUnit(newUnit: RepeatUnitType) {
    if (component === undefined || props === undefined) return;
    props.unit = newUnit;
    component.props = props;
  }

  function selectDurationUnit(newUnit: RepeatRelativeUnitType) {
    if (component === undefined || props === undefined) return;
    props.durationUnit = newUnit;
    component.props = props;
  }

  function getConstraintUnit(
    constraint: keyof RepeatConstraints,
    full: boolean = false
  ) {
    if (component === undefined) return "";
    if (constraint === "repetitions") return "";
    const unit = component.props.constraintUnit(constraint);
    return (unit === "YYYY-MM-DD HH:MM:SS" || unit === "HH:MM:SS") && !full
      ? ""
      : unit;
  }
</script>

<PaneSection title="Repetition" name={"properties-repitition"}>
  {#if component !== undefined && props !== undefined}
    {#each Object.entries(props.constraints) as [constraint, value]}
      <label class="horizontal">
        {capitalize(constraint)}:
        <input
          type={component.props.isUnitNumeric(constraint) ? "number" : "text"}
          step={component.props.isUnitNumeric(constraint) ? "any" : undefined}
          min={component.props.isUnitNumeric(constraint) ? 0 : undefined}
          value={component.props.toUnitString(value, constraint)}
          on:change={(event) => updateConstraint(event, constraint)}
          disabled={!selectedConstraints.includes(constraint)}
          placeholder={constraint === "repetitions"
            ? "Number of repetitions"
            : `${capitalize(constraint)}${constraint === "start" || constraint === "end" ? " time" : ""} in ${getConstraintUnit(constraint, true)}`}
        />
        {getConstraintUnit(constraint)}
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
    <SelectDropdown
      label={`Time unit`}
      buttonTitle={`Time unit options`}
      dropdownText={component.props.unit}
    >
      <svelte:fragment slot="buttons" let:toggleDropdown>
        {#each getUnitOptions() as option}
          {#if component.props.unit !== option}
            <button
              on:click={() => {
                selectUnit(option);
                toggleDropdown();
              }}>{option}</button
            >
          {/if}
        {/each}
      </svelte:fragment>
    </SelectDropdown>
    <SelectDropdown
      label={`Duration unit`}
      buttonTitle={`Duration unit options`}
      dropdownText={component.props.durationUnit}
    >
      <svelte:fragment slot="buttons" let:toggleDropdown>
        {#each getDurationUnitOptions() as option}
          {#if component.props.durationUnit !== option}
            <button
              on:click={() => {
                selectDurationUnit(option);
                toggleDropdown();
              }}>{option}</button
            >
          {/if}
        {/each}
      </svelte:fragment>
    </SelectDropdown>
    <SelectDropdown
      label={`Layering mode`}
      buttonTitle={`Layering mode options`}
      dropdownText={LayerMode[component.layerMode]}
    >
      <svelte:fragment slot="buttons" let:toggleDropdown>
        {#each [LayerMode.Override, LayerMode.Coincide] as option}
          {#if component.layerMode !== option}
            <button
              on:click={() => {
                if (component === undefined) return;
                component.layerMode = option;
                $updateIndex++;
                toggleDropdown();
              }}>{LayerMode[option]}</button
            >
          {/if}
        {/each}
      </svelte:fragment>
    </SelectDropdown>
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
