<script lang="ts">
  import TimescaleLabel from "./TimescaleLabel.svelte";
  import { Timescale, TimescaleSet } from "./timescale";

  export let offset: number;
  export let duration: number;
  export let timescale: Timescale;
  export let timescaleSet: TimescaleSet;

  let labelIntervals = timescale.getLabelIntervals(offset, offset + duration);
  let tickIntervals = timescaleSet
    .shiftIndex(timescale, 1)
    .getLabelIntervals(offset, offset + duration);
</script>

<div class="container">
  <div class="ticks-container">
    <div class="ticks">
      {#each tickIntervals as [tickOffset, label]}
        <TimescaleLabel
          label=""
          level={1}
          offset={tickOffset}
          tileOffset={offset}
          tileDuration={duration}
        />
      {/each}
    </div>
  </div>
  <div class="labels">
    {#each labelIntervals as [labelOffset, label]}
      <TimescaleLabel
        {label}
        offset={labelOffset}
        tileOffset={offset}
        tileDuration={duration}
      />
    {/each}
  </div>
</div>

<style>
  .container {
    position: relative;
  }

  .labels {
    position: relative;
    width: 100%;
  }

  .ticks-container {
    z-index: -1;
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
  }

  .ticks {
    position: relative;
    width: 100%;
  }
</style>
