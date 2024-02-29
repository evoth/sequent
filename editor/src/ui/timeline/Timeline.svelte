<script lang="ts">
  import { project } from "../../data/stores";
  import Tile from "./Tile.svelte";
  import { RelativeTimescales } from "./timescale";

  let width = 0;

  $: scale = $project.openedSequence!.scale;
  $: offset = $project.openedSequence!.offset;

  $: tileTimescale = getTileTimescale(scale);
  $: tileIntervals = tileTimescale.getIntervals(
    offset,
    offset + width / scale,
    tileTimescale.labelInterval,
    (offset) => String(offset / tileTimescale.labelInterval),
    true
  );

  $: tileIntervals, console.log(tileIntervals.length);

  function scroll(event: WheelEvent) {
    if (event.ctrlKey) {
      event.preventDefault();
      $project.openedSequence!.scale *= Math.pow(2, -event.deltaY / 2000);
    } else if (event.shiftKey) {
      $project.openedSequence!.offset += event.deltaY / scale;
      $project.openedSequence!.offset = Math.max(
        0,
        $project.openedSequence!.offset
      );
    }
  }

  function getTileTimescale(newScale: number) {
    const best = RelativeTimescales.bestTimescale(480, newScale);
    return RelativeTimescales.shiftIndex(best, -1) ?? best;
  }
</script>

<div class="container" on:wheel={scroll} bind:clientWidth={width}>
  <p>scale: {$project.openedSequence?.scale}</p>
  <p>offset: {offset}</p>
  <div class="tiles">
    {#each tileIntervals as [tileOffset, index] (tileOffset)}
      <Tile
        offset={tileOffset}
        duration={tileTimescale.labelInterval}
        index={Number(index)}
      />
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

  .tiles {
    position: relative;
    height: 100%;
  }
</style>
