<script lang="ts">
  import TimescaleLabel from "./TimescaleLabel.svelte";

  export let labelIntervals: [offset: number, label: string][];
  export let tickIntervals: [offset: number, label: string][];
</script>

<div class="container">
  <div class="ticks">
    {#each tickIntervals as [tickOffset, label], index}
      {#if index > 0}
        <TimescaleLabel label="" level={1} />
      {:else}
        <div style="width: 100%"></div>
      {/if}
    {/each}
  </div>
  <div class="labels">
    {#each labelIntervals as [labelOffset, label]}
      <TimescaleLabel {label} />
    {/each}
  </div>
</div>

<style>
  .container {
    position: relative;
  }

  .labels {
    display: flex;
  }

  .ticks {
    z-index: -1;
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    display: flex;
  }
</style>
