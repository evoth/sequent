<script lang="ts">
  import Portal from "svelte-portal";
  import { Component } from "../../data/sequence";
  import { project } from "../../data/stores";
  import ComponentBody from "./ComponentBody.svelte";

  export let getComponent: () => Component | undefined;
  export let hideChildOnDrag = false;

  let dragging: Component | undefined = undefined;

  let draggingBox: DOMRect;
  let dragOffset: [x: number, y: number];
  let draggingDuration: number;

  let outsideBounds = true;

  function dragStart(event: MouseEvent) {
    event.preventDefault();

    dragging = getComponent();

    // TODO: Error handling
    if (dragging === undefined) return;

    draggingDuration = dragging?.getDuration()[1] ?? 5;
    dragOffset = [event.offsetX, event.offsetY];
    dragMove(event);
  }

  function dragMove(event: MouseEvent) {
    const timelineBox = document
      .getElementById("timeline")!
      .getBoundingClientRect();

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

<!-- svelte-ignore a11y-no-static-element-interactions -->
<div on:mousedown={(event) => dragStart(event)}>
  {#if !(hideChildOnDrag && dragging !== undefined)}
    <slot />
  {/if}
</div>

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
