<script lang="ts">
  import { project } from "../../data/stores";
  import Tile from "./Tile.svelte";
  import TimelineTile from "./TimelineTile.svelte";
  import TimescaleLabel from "./TimescaleLabel.svelte";
  import { RelativeTimescales } from "./timescale";

  let width = 0;
  let containerElement: HTMLElement;

  $: $project.openedSequence, clampOffset(width);
  $: scale = $project.openedSequence!.scale;
  $: start = $project.openedSequence!.offset;
  $: end = start + width / scale;

  $: timescale = RelativeTimescales.bestTimescale(360, scale);
  $: titleIntervals = timescale.getTitleIntervals(start, end);
  $: timelineTileIntervals = timescale.getTileIntervals(start, end);

  function clampOffset(containerWidth: number) {
    console.log(containerWidth);
    if (containerWidth === 0) return;
    $project.openedSequence!.offset = Math.max(
      (containerWidth / $project.openedSequence!.scale) * -0.5,
      $project.openedSequence!.offset
    );
  }

  function scroll(event: WheelEvent) {
    if (event.ctrlKey) {
      event.preventDefault();
      const zoomDelta = Math.pow(2, -event.deltaY / 1500);
      $project.openedSequence!.offset +=
        (1 - 1 / zoomDelta) *
        ((event.clientX - containerElement.getBoundingClientRect().left) /
          scale);
      $project.openedSequence!.scale *= zoomDelta;
      clampOffset(width);
    } else if (event.shiftKey) {
      event.preventDefault();
      $project.openedSequence!.offset += event.deltaY / scale;
      clampOffset(width);
    }
  }
</script>

<div
  class="container"
  on:wheel={scroll}
  bind:clientWidth={width}
  bind:this={containerElement}
>
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
          <TimelineTile
            labelIntervals={timescale.getLabelIntervals(
              offset,
              offset + duration
            )}
            tickIntervals={RelativeTimescales.shiftIndex(
              timescale,
              1
            ).getLabelIntervals(offset, offset + duration)}
          />
        </svelte:fragment>
      </Tile>
    {/each}
  </div>
</div>

<style>
  .container {
    width: 100%;
    height: 100%;
    background-color: var(--gray-95);
    overflow-x: hidden;
    overflow-y: hidden;
  }

  .titleTiles {
    position: relative;
  }

  .timescaleTiles {
    top: 1.5rem;
    position: relative;
    height: 100%;
  }
</style>
