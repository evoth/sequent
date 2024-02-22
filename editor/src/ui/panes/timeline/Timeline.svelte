<script lang="ts">
  import { selectedSequence } from "../../../stores";
</script>

{#if $selectedSequence !== undefined}
  <div></div>
{/if}

<style>
  div {
    width: 100%;
    height: 100%;
    background-color: var(--gray-95);
  }
</style>
