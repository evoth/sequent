<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import { selectedComponents, updateIndex } from "../../data/stores";
  import Tile from "./Tile.svelte";
  import TimelineTile from "./TimelineTile.svelte";
  import TimescaleLabel from "./TimescaleLabel.svelte";
  import TimescaleTile from "./TimescaleTile.svelte";
  import {
    AbsoluteTimescales,
    LABEL_TIMESCALE_PX,
    RelativeTimescales,
  } from "./timescale";

  export let sequence: Sequence;

  let timelineElement: HTMLElement;

  let width = 0;
  let height = 0;

  $: sequence, clampOffset(width);
  $: sequence,
    clampScroll(height, sequence.layerHeight, sequence.layers.length);
  $: start = sequence.offset;
  $: end = start + width / sequence.scale;

  $: timescaleSet = sequence.isAbsolute
    ? AbsoluteTimescales
    : RelativeTimescales;
  $: timescale = timescaleSet.bestTimescale(LABEL_TIMESCALE_PX, sequence.scale);
  $: titleIntervals = timescale.getTitleIntervals(start, end);
  $: timelineTileIntervals = timescale.getTileIntervals(start, end);
  $: tilesEnd = getTilesEnd(timelineTileIntervals);

  function getTilesEnd(newTimelineTileIntervals: [number, number, number][]) {
    if (newTimelineTileIntervals.length === 0) {
      return 0;
    } else {
      const lastInterval =
        newTimelineTileIntervals[newTimelineTileIntervals.length - 1];
      return lastInterval[0] + lastInterval[2];
    }
  }

  function clampOffset(containerWidth: number) {
    if (containerWidth === 0) return;
    sequence.offset = Math.max(
      (containerWidth / sequence.scale) * -0.5,
      sequence.offset
    );
  }

  function clampScroll(
    containerHeight: number,
    layerHeight: number,
    numLayers: number
  ) {
    if (containerHeight === 0) return;
    sequence.scroll = Math.max(
      containerHeight * -0.5 + layerHeight,
      Math.min(
        containerHeight * 0.5 + layerHeight * (numLayers - 1),
        sequence.scroll
      )
    );
  }

  // TODO: make rates adjustable
  function handleScroll(event: WheelEvent) {
    event.preventDefault();
    if (event.ctrlKey) {
      const zoomDelta = Math.pow(2, -event.deltaY / 1500);
      sequence.offset +=
        (1 - 1 / zoomDelta) *
        ((event.clientX - timelineElement.getBoundingClientRect().left) /
          sequence.scale);
      sequence.scale *= zoomDelta;
      clampOffset(width);
    } else if (event.shiftKey) {
      sequence.offset += (event.deltaY / sequence.scale) * 0.75;
      clampOffset(width);
    } else if (event.altKey) {
      // TODO: Center zoom on mouse
      sequence.layerHeight -= event.deltaY / 10;
      sequence.layerHeight = Math.max(100, Math.min(300, sequence.layerHeight));
    } else {
      sequence.scroll -= event.deltaY / 2;
      clampScroll(height, sequence.layerHeight, sequence.layers.length);
    }
  }

  function getTitleTileDuration(
    tileOffset: number,
    tileDuration: number | undefined
  ) {
    const newOffset = Math.max(start, tileOffset);
    const tempDuration = tileDuration ?? Number.MAX_VALUE;
    return Math.min(
      end - Math.max(start, newOffset),
      tempDuration,
      tileOffset + tempDuration - newOffset
    );
  }
</script>

<!-- svelte-ignore a11y-no-static-element-interactions -->
<!-- svelte-ignore a11y-click-events-have-key-events -->
<!-- svelte-ignore a11y-click-events-have-key-events -->
<div
  id="timeline"
  class="container"
  bind:this={timelineElement}
  on:wheel={handleScroll}
  bind:clientWidth={width}
  bind:clientHeight={height}
  on:click={() => {
    $selectedComponents.set(sequence, undefined);
    $selectedComponents = $selectedComponents;
  }}
>
  <div
    class="timelineTiles"
    style:transform={`translateY(${sequence.scroll}px)`}
    style:height={`${sequence.layerHeight}px`}
  >
    {#each timelineTileIntervals.toReversed() as [tileOffset, _, duration], tileIndex (`${tileOffset} ${timescale.tileInterval} ${$updateIndex} ${sequence.id}`)}
      <Tile offset={tileOffset} {duration} {sequence}>
        <TimelineTile
          offset={tileOffset}
          {duration}
          bind:sequence
          tileIndex={timelineTileIntervals.length - tileIndex - 1}
          {tilesEnd}
        />
      </Tile>
    {/each}
  </div>
  <div class="timescaleBg"></div>
  <div class="titleTiles">
    {#each titleIntervals as [tileOffset, title, duration] (`${tileOffset} ${timescale.titleInterval}`)}
      <Tile
        offset={Math.max(start, tileOffset)}
        duration={getTitleTileDuration(tileOffset, duration)}
        {sequence}
      >
        <TimescaleLabel
          label={title}
          offset={tileOffset}
          {tileOffset}
          tileDuration={getTitleTileDuration(tileOffset, duration)}
          align={getTitleTileDuration(tileOffset, duration) < end - start
            ? "left"
            : "center"}
        />
      </Tile>
    {/each}
  </div>
  <div class="timescaleTiles">
    {#each timelineTileIntervals as [tileOffset, index, duration] (`${tileOffset} ${timescale.tileInterval}`)}
      <Tile offset={tileOffset} {duration} {sequence}>
        <TimescaleTile
          offset={tileOffset}
          {duration}
          {timescale}
          {timescaleSet}
        />
      </Tile>
    {/each}
  </div>
</div>

<style>
  .container {
    flex: 1;
    width: 100%;
    background-color: var(--gray-95);
    overflow-x: hidden;
    overflow-y: hidden;
    position: relative;
  }

  .timescaleBg {
    position: absolute;
    width: 100%;
    height: 3rem;
    background-color: color-mix(in srgb, var(--gray-95) 50%, transparent);
  }

  .titleTiles {
    position: relative;
  }

  .timescaleTiles {
    top: 1.5rem;
    position: relative;
  }

  .timelineTiles {
    position: absolute;
    width: 100%;
    bottom: 50%;
  }
</style>
