<script lang="ts">
  import { Action } from "../../../data/action";
  import { Manager } from "../../../data/manager";
  import { RepeatProps } from "../../../data/repeat";
  import { Component, LayerMode, Sequence } from "../../../data/sequence";
  import { project } from "../../../data/stores";
  import { Timestamp } from "../../../data/timestamp";
  import Draggable from "../../timeline/Draggable.svelte";
  import PaneSection from "../PaneSection.svelte";

  export let title: string;
  export let name: string;
  export let manager: Manager<Action | Sequence>;

  function getComponent(component: Action | Sequence): Component | undefined {
    if ($project.openedSequence === undefined) return;

    const repeatProps = new RepeatProps(
      {
        start: new Timestamp(
          $project.timestampManager,
          0,
          $project.openedSequence.rootTimestamp
        ),
        repetitions: 1,
        interval: 5,
      },
      ["start", "repetitions", "interval"]
    );

    if (component instanceof Action) {
      return new Component(
        component.newState(),
        repeatProps,
        LayerMode.Coincide,
        $project.openedSequence.rootTimestamp
      );
    } else if (component instanceof Sequence) {
      return new Component(
        component,
        repeatProps,
        LayerMode.Coincide,
        $project.openedSequence.rootTimestamp
      );
    }
  }
</script>

<PaneSection {title} {name}>
  {#each manager.children.entries() as [id, component] (id)}
    <!-- TODO: disallow any sequences which depend on current sequence -->
    {#if component !== $project.openedSequence}
      <Draggable getComponent={() => getComponent(component)}>
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
