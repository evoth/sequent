<script lang="ts">
  import { project } from "../../data/stores";
  import Tile from "./Tile.svelte";
  import TimescaleLabel from "./TimescaleLabel.svelte";
  import TimelineTile from "./TimescaleTile.svelte";
  import { RelativeTimescales } from "./timescale";

  let width = 0;
  let height = 0;
  let containerElement: HTMLElement;

  $: $project.openedSequence, clampOffset(width);
  $: $project.openedSequence, clampScroll(height);
  $: scale = $project.openedSequence!.scale;
  $: start = $project.openedSequence!.offset;
  $: end = start + width / scale;

  $: scroll = $project.openedSequence!.scroll;
  $: layerHeight = $project.openedSequence!.layerHeight;

  $: timescale = RelativeTimescales.bestTimescale(360, scale);
  $: titleIntervals = timescale.getTitleIntervals(start, end);
  $: timelineTileIntervals = timescale.getTileIntervals(start, end);

  function clampOffset(containerWidth: number) {
    if (containerWidth === 0) return;
    $project.openedSequence!.offset = Math.max(
      (containerWidth / $project.openedSequence!.scale) * -0.5,
      $project.openedSequence!.offset
    );
  }

  // TODO: update to account for height of layers (if there are a bunch of layers, should be able to scroll bottom of layers to top of container)
  function clampScroll(containerHeight: number) {
    if (containerHeight === 0) return;
    $project.openedSequence!.scroll = Math.max(
      containerHeight * -0.5,
      Math.min(containerHeight * 0.5, $project.openedSequence!.scroll)
    );
  }

  // TODO: make rates adjustable
  function handleScroll(event: WheelEvent) {
    event.preventDefault();
    if (event.ctrlKey) {
      const zoomDelta = Math.pow(2, -event.deltaY / 1500);
      $project.openedSequence!.offset +=
        (1 - 1 / zoomDelta) *
        ((event.clientX - containerElement.getBoundingClientRect().left) /
          scale);
      $project.openedSequence!.scale *= zoomDelta;
      clampOffset(width);
    } else if (event.shiftKey) {
      $project.openedSequence!.offset += event.deltaY / scale;
      clampOffset(width);
    } else if (event.altKey) {
      $project.openedSequence!.layerHeight -= event.deltaY / 10;
      $project.openedSequence!.layerHeight = Math.max(
        100,
        Math.min(300, $project.openedSequence!.layerHeight)
      );
    } else {
      $project.openedSequence!.scroll -= event.deltaY / 2;
      clampScroll(height);
    }
  }
</script>

<div
  class="container"
  on:wheel={handleScroll}
  bind:clientWidth={width}
  bind:clientHeight={height}
  bind:this={containerElement}
>
  <div
    class="timelineTiles"
    style:transform={`translateY(calc(-50% + ${scroll}px))`}
    style:height={`${layerHeight}px`}
  >
    {#each timelineTileIntervals as [tileOffset, index] (`${tileOffset} ${timescale.tileInterval}`)}
      <Tile offset={tileOffset} duration={timescale.tileInterval} {timescale}>
        <div
          style="background-color: var(--gray-90); height: 100%; width: 100%;"
        ></div>
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
        {timescale}
      >
        <TimescaleLabel label={title} offset={tileOffset} />
      </Tile>
    {/each}
  </div>
  <div class="timescaleTiles">
    {#each timelineTileIntervals as [tileOffset, index] (`${tileOffset} ${timescale.tileInterval}`)}
      <Tile offset={tileOffset} duration={timescale.tileInterval} {timescale}>
        <svelte:fragment let:offset let:duration let:timescale>
          <TimelineTile {offset} {duration} {timescale} />
        </svelte:fragment>
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
    top: 50%;
  }
</style>
