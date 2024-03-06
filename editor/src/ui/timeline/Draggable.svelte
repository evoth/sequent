<script lang="ts">
  import Portal from "svelte-portal";
  import { Component, Layer, Sequence } from "../../data/sequence";
  import { updateIndex } from "../../data/stores";
  import ComponentBody from "./ComponentBody.svelte";

  export let getComponent: () => Component | undefined;
  export let hideChildOnDrag = false;
  export let sequence: Sequence | undefined;
  export let removeComponent: () => void;

  let dragging: Component | undefined = undefined;

  let draggingBox: DOMRect;
  let dragOffset: [x: number, y: number];
  let draggingDuration: number;

  let previewBox: DOMRect;
  let previewLayer: number;
  let previewOffset: number;

  let outsideBounds = true;

  function dragStart(event: MouseEvent) {
    event.preventDefault();

    if (sequence === undefined) return;

    dragging = getComponent();

    // TODO: Error handling
    if (dragging === undefined) return;

    draggingDuration = dragging?.getDuration()[1] ?? 5;
    // TODO: account for padding
    dragOffset = [event.offsetX, event.offsetY];
    dragMove(event);

    sequence.layers.push(new Layer([], sequence.rootTimestamp));
    sequence.layers = sequence.layers;
  }

  function dragMove(event: MouseEvent) {
    if (sequence === undefined) return;

    const timelineBox = document
      .getElementById("timeline")!
      .getBoundingClientRect();

    let width = draggingDuration * sequence.scale;
    let height = sequence.layerHeight;
    let x = event.clientX - Math.min(width, dragOffset[0]);
    let y = event.clientY - Math.min(height, dragOffset[1]);

    // TODO: Handle infinite sequences (starting at negative infinity)
    draggingBox = new DOMRect(x, y, width, height);

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

    const timelineX = x - timelineBox.left;
    const timelineY = y - timelineBox.top;

    previewLayer = Math.round(
      (timelineBox.height / 2 + sequence.scroll - timelineY) /
        sequence.layerHeight -
        1
    );
    previewLayer = Math.max(
      0,
      Math.min(sequence.layers.length - 1, previewLayer)
    );

    previewOffset = timelineX / sequence.scale + sequence.offset;

    previewBox = new DOMRect(
      x,
      timelineBox.height / 2 +
        timelineBox.top -
        (previewLayer + 1) * sequence.layerHeight +
        sequence.scroll,
      width,
      height
    );
  }

  function dragEnd(event: MouseEvent) {
    if (sequence === undefined || dragging === undefined) return;

    removeComponent();

    // TODO: Should probably have error handling here
    if (!outsideBounds) {
      dragging.props.constraints.start!.value = previewOffset;
      sequence.layers[previewLayer].children.add(dragging);
    }

    const emptyLayers = new Set<Layer>();
    for (const layer of sequence.layers) {
      if (layer.children.size === 0) {
        emptyLayers.add(layer);
      }
    }
    sequence.layers = sequence.layers.filter((l) => !emptyLayers.has(l));

    $updateIndex++;

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
      {#if !outsideBounds}
        <div
          class="previewContainer"
          style:left={`${previewBox.left}px`}
          style:top={`${previewBox.top}px`}
          style:width={`${previewBox.width}px`}
          style:height={`${previewBox.height}px`}
        >
          <ComponentBody component={dragging} />
        </div>
      {/if}
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

  .previewContainer {
    position: absolute;
    opacity: 50%;
  }
</style>
