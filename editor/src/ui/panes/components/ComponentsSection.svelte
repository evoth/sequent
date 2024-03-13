<script lang="ts">
  import { Action } from "../../../data/action";
  import { Manager } from "../../../data/manager";
  import { RepeatProps } from "../../../data/repeat";
  import { Component, LayerMode, Sequence } from "../../../data/sequence";
  import { project } from "../../../data/stores";
  import Draggable from "../../timeline/Draggable.svelte";
  import PaneSection from "../PaneSection.svelte";

  export let title: string;
  export let name: string;
  export let manager: Manager<Action | Sequence>;

  $: dependants = $project.openedSequence?.getDependants() ?? [];

  function getComponent(component: Action | Sequence): Component | undefined {
    if ($project.openedSequence === undefined) return;

    const repeatProps = new RepeatProps(
      {
        start: 0,
        repetitions: 1,
        interval: 5,
      },
      ["start", "repetitions", "interval"]
    );

    if (component instanceof Action) {
      return new Component(
        component.newState(),
        repeatProps,
        LayerMode.Coincide
      );
    } else if (component instanceof Sequence) {
      return new Component(component, repeatProps, LayerMode.Coincide);
    }
  }
</script>

<PaneSection {title} {name}>
  {#each manager.children.entries() as [id, component] (id)}
    <!-- TODO: disallow any sequences which depend on current sequence -->
    {#if !(component instanceof Sequence && (component === $project.openedSequence || dependants.includes(component) || (component.getDuration() ?? 0) === 0))}
      <Draggable
        getComponent={() => getComponent(component)}
        bind:sequence={$project.openedSequence}
        removeComponent={() => {}}
      >
        <div
          class="component"
          style:background-color={`hsl(${component.hue}deg var(--component-saturation) var(--component-lightness))`}
          class:disabled={$project.openedSequence === undefined}
        >
          {component.name}
        </div>
      </Draggable>
    {/if}
  {/each}
</PaneSection>

<style>
  .component {
    padding: 1rem;
    border-radius: 1rem;
    font-weight: 500;
    cursor: pointer;
  }

  .disabled {
    cursor: not-allowed;
  }
</style>
