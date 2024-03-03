<script lang="ts">
  import { clickoutside } from "@svelte-put/clickoutside";
  import { fade } from "svelte/transition";

  export let closeCondition = false;
  export let align: "top" | "bottom" = "bottom";
  export let showOptions = false;

  const openDropdown = () => (showOptions = !showOptions);

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
    <div
      class="menu-container"
      transition:fade={{ duration: 100 }}
      style={align === "top" ? "top: 0.5rem" : "top: 100%"}
    >
      <div class="options-menu">
        <slot name="buttons" />
      </div>
    </div>
  {/if}
  <slot />
</div>

<style>
  .options-container {
    position: relative;
    z-index: 1;
  }

  .menu-container {
    position: absolute;
    left: 0;
    min-width: 200px;
  }

  .options-menu {
    display: inline-flex;
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
