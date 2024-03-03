<script lang="ts">
  import { Sequence } from "../../data/sequence";

  export let offset: number;
  export let duration: number;
  export let sequence: Sequence;

  $: tileOffset = (offset - sequence.offset) * sequence.scale;
  $: tileWidth = duration * sequence.scale;
</script>

<div
  class="container"
  style:transform={`translateX(${tileOffset}px)`}
  style:width={`${tileWidth}px`}
>
  <slot />
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
