<script lang="ts">
  import Portal from "svelte-portal";
  import { Action } from "../../../data/action";
  import { Manager } from "../../../data/manager";
  import { RepeatProps } from "../../../data/repeat";
  import { Component, LayerMode, Sequence } from "../../../data/sequence";
  import { project } from "../../../data/stores";
  import { Timestamp } from "../../../data/timestamp";
  import ComponentBody from "../../timeline/ComponentBody.svelte";
  import PaneSection from "../PaneSection.svelte";

  export let title: string;
  export let name: string;
  export let manager: Manager<Action | Sequence>;
  export let timelineElement: HTMLElement;

  let dragging: Component | undefined = undefined;

  // TODO: Error handling
  $: draggingDuration = dragging?.getDuration()[1] ?? 5;

  let draggingBox: DOMRect;
  let dragOffset: [x: number, y: number];

  let outsideBounds = true;

  function dragStart(event: MouseEvent, component: Action | Sequence) {
    event.preventDefault();

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
      dragging = new Component(
        component.newState(),
        repeatProps,
        LayerMode.Coincide,
        $project.openedSequence.rootTimestamp
      );
    } else if (component instanceof Sequence) {
      dragging = new Component(
        component,
        repeatProps,
        LayerMode.Coincide,
        $project.openedSequence.rootTimestamp
      );
    }

    dragOffset = [event.offsetX, event.offsetY];

    dragMove(event);
  }

  function dragMove(event: MouseEvent) {
    const timelineBox = timelineElement.getBoundingClientRect();

    let width = draggingDuration * $project.openedSequence!.scale;
    let height = $project.openedSequence!.layerHeight;

    draggingBox = new DOMRect(
      event.clientX - Math.min(width, dragOffset[0]),
      event.clientY - Math.min(height, dragOffset[1]),
      width,
      height
    );

    if (
      event.clientX < timelineBox.left ||
      event.clientX > timelineBox.right ||
      event.clientY < timelineBox.top ||
      event.clientY > timelineBox.bottom
    ) {
      outsideBounds = true;
      return;
    }
    outsideBounds = false;
  }

  function dragEnd(event: MouseEvent) {
    dragging = undefined;
  }
</script>

<PaneSection {title} {name}>
  {#each manager.children.entries() as [id, component] (id)}
    <!-- svelte-ignore a11y-no-static-element-interactions -->
    <!-- TODO: disallow any sequences which depend on current sequence -->
    {#if component !== $project.openedSequence}
      <div
        class="component"
        on:mousedown={(event) => dragStart(event, component)}
        style:background-color={`hsl(${component.hue}deg var(--component-saturation) var(--component-lightness))`}
        class:disabled={$project.openedSequence === undefined}
      >
        {component.name}
      </div>
    {/if}
  {/each}
</PaneSection>

{#if dragging !== undefined}
  <Portal target="body">
    <!-- svelte-ignore a11y-no-static-element-interactions -->
    <div class="dragArea" on:mousemove={dragMove} on:mouseup={dragEnd}>
      <div
        class="dragContainer"
        style:left={`${draggingBox.left}px`}
        style:top={`${draggingBox.top}px`}
        style:width={`${draggingBox.width}px`}
        style:height={`${draggingBox.height}px`}
      >
        <ComponentBody component={dragging} disabled={outsideBounds} shadow />
      </div>
    </div>
  </Portal>
{/if}

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

  .dragArea {
    position: fixed;
    top: 0;
    bottom: 0;
    right: 0;
    left: 0;
  }

  .dragContainer {
    position: absolute;
  }
</style>
