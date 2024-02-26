<script lang="ts">
  import { clickoutside } from "@svelte-put/clickoutside";
  import { fade } from "svelte/transition";

  export let closeCondition: boolean;

  const openDropdown = () => (showOptions = true);
  let showOptions = false;

  // Disables showOptions when closeCondition is true
  $: showOptions = !closeCondition && showOptions;
</script>

<div
  class="options-container"
  use:clickoutside
  on:clickoutside={() => (showOptions = false)}
>
  <slot name="button" {openDropdown} />
  {#if showOptions}
    <div class="options-menu" transition:fade={{ duration: 100 }}>
      <slot name="buttons" />
    </div>
  {/if}
</div>

<style>
  .options-container {
    position: relative;
    z-index: 1;
  }

  .options-menu {
    position: absolute;
    top: 0.5rem;
    left: 0;
    display: flex;
    flex-direction: column;
    padding: 0.5rem;
    background-color: var(--gray-90);
    border-radius: 0.7rem;
    border: var(--border-style);
    box-shadow:
      0 10px 15px -3px var(--shadow),
      0 4px 6px -4px var(--shadow);
  }

  .options-menu > :global(button) {
    font-size: 1.2rem;
    padding: 0.3rem;
    text-align: start;
  }
</style>
