<script lang="ts">
  import { project } from "../../data/stores";
  import { RelativeTimescales } from "./timescale";

  export let offset: number;
  export let duration: number;
  // Testing mostly
  export let index: number;

  $: tileOffset =
    (offset - $project.openedSequence!.offset) * $project.openedSequence!.scale;
  $: tileWidth = duration * $project.openedSequence!.scale;

  let timescale = RelativeTimescales.bestTimescale(
    480,
    $project.openedSequence!.scale
  );

  // Testing
  let tileTimescale = RelativeTimescales.shiftIndex(timescale, -1) ?? timescale;
</script>

<div
  class="container"
  style:transform={`translateX(${tileOffset}px)`}
  style:width={`${tileWidth}px`}
  style:background-color={`var(--gray-${index % 2 == 0 ? "90" : "85"})`}
>
  <p>{tileTimescale.getTitle(offset)}{tileTimescale.getLabel(offset)}</p>
</div>

<style>
  .container {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: var(--gray-95);
  }
</style>
