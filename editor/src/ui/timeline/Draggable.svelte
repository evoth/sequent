<script lang="ts">
  import Portal from "svelte-portal";
  import { Component, Layer, LayerError, Sequence } from "../../data/sequence";
  import { selectedComponents, updateIndex } from "../../data/stores";
  import ComponentBody from "./ComponentBody.svelte";
  import {
    AbsoluteTimescales,
    LABEL_TIMESCALE_PX,
    RelativeTimescales,
  } from "./timescale";

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
  let previewNoSnap: boolean;

  let snapPoints: [offset: number, pixels: number][];
  let snapLayer: number | undefined;

  let outsideBounds = true;

  let originalBox: DOMRect;
  let isDragging: boolean;
  $: addRemoveExtraLayer(isDragging);

  const timescaleSet = sequence?.isAbsolute
    ? AbsoluteTimescales
    : RelativeTimescales;

  const ENDPOINT_SNAP_PX = 25;
  const TICK_SNAP_PX = 0;
  const MIN_DRAG_DELTA = 10;

  function getSnapPoints(
    layerIndex: number,
    timelineBox: DOMRect
  ): [offset: number, pixels: number][] {
    if (sequence === undefined || draggingDuration === undefined) return [];

    const timelineStart = Math.max(0, sequence.offset);
    const timelineEnd = sequence.offset + timelineBox.width / sequence.scale;
    const offsetValid = (offset: number) =>
      offset + draggingDuration > timelineStart && offset < timelineEnd;
    const tickTimescale = timescaleSet.shiftIndex(
      timescaleSet.bestTimescale(LABEL_TIMESCALE_PX, sequence.scale),
      2
    );
    const tickSnaps = (
      start: number,
      end: number
    ): [offset: number, pixels: number][] => {
      const tickIntervals = tickTimescale.getLabelIntervals(start, end);
      return tickIntervals
        .map(([offset, label]): [number, number] => [offset, TICK_SNAP_PX])
        .filter(([offset, pixels]) => offsetValid(offset));
    };

    const layerValidation = sequence.layers[layerIndex].validate(dragging);
    if (layerValidation.error === LayerError.Empty)
      return tickSnaps(
        Math.max(0, timelineStart - draggingDuration),
        timelineEnd
      );
    if (layerValidation.childBounds === undefined) return [];

    let snaps: [offset: number, pixels: number][] = [];
    let prevEnd: number | undefined = 0;
    for (const [start, end] of layerValidation.childBounds) {
      if (prevEnd === undefined) break;
      const startOffset = start;
      const endOffset = end;

      if (
        startOffset !== undefined &&
        startOffset - prevEnd >= draggingDuration
      ) {
        if (offsetValid(prevEnd)) {
          snaps.push([prevEnd, ENDPOINT_SNAP_PX]);
        }
        if (offsetValid(startOffset - draggingDuration)) {
          snaps.push([startOffset - draggingDuration, ENDPOINT_SNAP_PX]);
        }
        snaps = [
          ...snaps,
          ...tickSnaps(
            Math.max(timelineStart, prevEnd),
            Math.min(timelineEnd, startOffset - draggingDuration)
          ),
        ];
      }

      prevEnd = endOffset;
    }

    if (prevEnd !== undefined && offsetValid(prevEnd)) {
      snaps = [
        ...snaps,
        [prevEnd, ENDPOINT_SNAP_PX],
        ...tickSnaps(Math.max(timelineStart, prevEnd), timelineEnd),
      ];
    }

    return snaps;
  }

  function snap(
    offset: number,
    snaps: [offset: number, pixels: number][]
  ): number | undefined {
    if (sequence === undefined) return undefined;

    let snapOffset: number | undefined = undefined;
    let snapPixels = 0;

    for (const [sOffset, sPixels] of snaps) {
      const diff = Math.abs(offset - sOffset);
      if (diff <= sPixels / sequence.scale && sPixels > snapPixels) {
        snapOffset = sOffset;
        snapPixels = sPixels;
      } else if (
        (snapOffset === undefined || diff < Math.abs(offset - snapOffset)) &&
        snapPixels === 0
      ) {
        snapOffset = sOffset;
      }
    }

    return snapOffset;
  }

  function hasBeenDragged(event: MouseEvent): boolean {
    return (
      Math.abs(event.clientX - (originalBox.x + dragOffset[0])) >=
        MIN_DRAG_DELTA ||
      Math.abs(event.clientY - (originalBox.y + dragOffset[1])) >=
        MIN_DRAG_DELTA
    );
  }

  function addRemoveExtraLayer(newIsDragging: boolean) {
    if (sequence === undefined) return;
    if (newIsDragging) {
      sequence.layers.push(new Layer([]));
      sequence.layers = sequence.layers;
    } else {
      sequence.layers = sequence.layers.filter(
        (layer) => layer.children.size > 0
      );
    }
  }

  function dragStart(event: MouseEvent) {
    event.preventDefault();

    dragging = getComponent();

    // TODO: Error handling
    if (sequence === undefined || dragging === undefined) return;

    $selectedComponents.set(sequence, dragging);

    draggingDuration = dragging?.getDuration()[1] ?? 5;
    originalBox = (event.target as HTMLElement)
      .closest(".dragParent > *")!
      .getBoundingClientRect();
    dragOffset = [event.clientX - originalBox.x, event.clientY - originalBox.y];
    snapLayer = undefined;
    isDragging = false;
    dragMove(event);
  }

  function dragMove(event: MouseEvent) {
    if (sequence === undefined) return;

    let width = draggingDuration * sequence.scale;
    let height = sequence.layerHeight;

    isDragging = hasBeenDragged(event);
    const DRAG_EDGE_BUFFER = 20;

    // TODO: If width is less that buffer * 2, offset should be width / 2
    let offsetX = dragOffset[0];
    if (width < DRAG_EDGE_BUFFER * 2) {
      offsetX = width / 2;
    } else {
      offsetX = Math.min(
        width - DRAG_EDGE_BUFFER,
        Math.max(DRAG_EDGE_BUFFER, offsetX)
      );
    }
    let x = isDragging ? event.clientX - offsetX : originalBox.x;

    let offsetY = dragOffset[1];
    offsetY = Math.min(
      height - DRAG_EDGE_BUFFER,
      Math.max(DRAG_EDGE_BUFFER, offsetY)
    );
    let y = isDragging ? event.clientY - offsetY : originalBox.y;

    // TODO: Handle infinite sequences (starting at negative infinity)
    draggingBox = new DOMRect(x, y, width, height);

    const timelineBox = document
      .getElementById("timeline")!
      .getBoundingClientRect();

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

    if (snapLayer !== previewLayer) {
      snapLayer = previewLayer;
      snapPoints = getSnapPoints(snapLayer, timelineBox);
    }

    const snappedOffset = snap(
      timelineX / sequence.scale + sequence.offset,
      snapPoints
    );
    if (snappedOffset === undefined) {
      previewNoSnap = true;
      return;
    } else {
      previewNoSnap = false;
    }

    previewOffset = snappedOffset;

    previewBox = new DOMRect(
      timelineBox.left + (previewOffset - sequence.offset) * sequence.scale,
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

    if (outsideBounds) {
      $selectedComponents.set(sequence, undefined);
    }
    if (!previewNoSnap) {
      removeComponent();
    }

    // TODO: Should probably have error handling here
    if (!outsideBounds && !previewNoSnap) {
      if (dragging.props.selectedConstraints.includes("end")) {
        dragging.props.constraints.end! +=
          previewOffset - dragging.props.constraints.start!;
      }
      dragging.props.constraints.start! = previewOffset;
      sequence.layers[previewLayer].children.add(dragging);
    }

    $updateIndex++;

    dragging = undefined;
    isDragging = false;
    addRemoveExtraLayer(isDragging);
  }
</script>

<!-- svelte-ignore a11y-no-static-element-interactions -->
<div class="dragParent" on:mousedown={(event) => dragStart(event)}>
  {#if !(hideChildOnDrag && isDragging)}
    <slot />
  {/if}
</div>

{#if dragging !== undefined}
  <Portal target="body">
    <!-- svelte-ignore a11y-no-static-element-interactions -->
    <div class="dragArea" on:mousemove={dragMove} on:mouseup={dragEnd}>
      {#if isDragging}
        {#if !outsideBounds && !previewNoSnap}
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
          <ComponentBody
            component={dragging}
            disabled={outsideBounds || previewNoSnap}
            shadow={isDragging}
            highlight
          />
        </div>
      {/if}
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
    overflow: hidden;
  }

  .dragContainer {
    position: absolute;
  }

  .previewContainer {
    position: absolute;
    opacity: 50%;
  }
</style>
