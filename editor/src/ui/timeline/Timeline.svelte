<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import { updateIndex } from "../../data/stores";
  import Tile from "./Tile.svelte";
  import TimelineTile from "./TimelineTile.svelte";
  import TimescaleLabel from "./TimescaleLabel.svelte";
  import TimescaleTile from "./TimescaleTile.svelte";
  import { RelativeTimescales } from "./timescale";

  export let sequence: Sequence;

  let timelineElement: HTMLElement;

  let width = 0;
  let height = 0;

  $: sequence, clampOffset(width);
  $: sequence,
    clampScroll(height, sequence.layerHeight, sequence.layers.length);
  $: start = sequence.offset;
  $: end = start + width / sequence.scale;

  $: timescale = RelativeTimescales.bestTimescale(360, sequence.scale);
  $: titleIntervals = timescale.getTitleIntervals(start, end);
  $: timelineTileIntervals = timescale.getTileIntervals(start, end);
  $: tilesEnd =
    timelineTileIntervals.length === 0
      ? 0
      : timelineTileIntervals[timelineTileIntervals.length - 1][0] +
        timescale.tileInterval;

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
</script>

<div
  id="timeline"
  class="container"
  bind:this={timelineElement}
  on:wheel={handleScroll}
  bind:clientWidth={width}
  bind:clientHeight={height}
>
  <div
    class="timelineTiles"
    style:transform={`translateY(${sequence.scroll}px)`}
    style:height={`${sequence.layerHeight}px`}
  >
    {#each timelineTileIntervals.toReversed() as [tileOffset, _], tileIndex (`${tileOffset} ${timescale.tileInterval} ${$updateIndex}`)}
      <Tile offset={tileOffset} duration={timescale.tileInterval} {sequence}>
        <TimelineTile
          offset={tileOffset}
          duration={timescale.tileInterval}
          bind:sequence
          tileIndex={timelineTileIntervals.length - tileIndex - 1}
          {tilesEnd}
        />
      </Tile>
    {/each}
  </div>
  <div class="timescaleBg"></div>
  <div class="titleTiles">
    {#each titleIntervals as [tileOffset, title] (`${tileOffset} ${timescale.titleInterval}`)}
      <Tile
        offset={Math.max(start, tileOffset)}
        duration={Math.min(
          end - Math.max(start, tileOffset),
          timescale.titleInterval ?? Number.MAX_VALUE
        )}
        {sequence}
      >
        <TimescaleLabel label={title} offset={tileOffset} />
      </Tile>
    {/each}
  </div>
  <div class="timescaleTiles">
    {#each timelineTileIntervals as [tileOffset, index] (`${tileOffset} ${timescale.tileInterval}`)}
      <Tile offset={tileOffset} duration={timescale.tileInterval} {sequence}>
        <TimescaleTile
          offset={tileOffset}
          duration={timescale.tileInterval}
          {timescale}
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
