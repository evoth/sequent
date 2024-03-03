<script lang="ts">
  import { project } from "../../data/stores";
  import type { Timescale } from "./timescale";

  export let offset: number;
  export let duration: number;
  export let timescale: Timescale;

  $: tileOffset =
    (offset - $project.openedSequence!.offset) * $project.openedSequence!.scale;
  $: tileWidth = duration * $project.openedSequence!.scale;
</script>

<div
  class="container"
  style:transform={`translateX(${tileOffset}px)`}
  style:width={`${tileWidth}px`}
>
  <slot {offset} {duration} {timescale} />
</div>

<style>
  .container {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
  }
</style>
