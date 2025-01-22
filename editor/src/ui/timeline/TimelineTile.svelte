<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import TileLayer from "./TileLayer.svelte";

  // TODO: Maybe just forgo the "TimelineTile" and go straight to the child component because we're just passing data through at this point
  // TODO: Can define a type for this and use rest parameters?
  export let offset: number;
  export let duration: number;
  export let sequence: Sequence;
  export let tileIndex: number;
  export let tilesEnd: number;
</script>

<div class="container">
  {#each sequence.layers as layer, index}
    <TileLayer
      {offset}
      {duration}
      {layer}
      {index}
      {tileIndex}
      {tilesEnd}
      bind:sequence
    />
  {/each}
</div>

<style>
  .container {
    position: relative;
    height: 100%;
    width: 100%;
  }
</style>
