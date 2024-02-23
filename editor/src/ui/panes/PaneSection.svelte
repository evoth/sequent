<script lang="ts">
  import { slide } from "svelte/transition";
  export let title: string;
  export let name: string;

  const storageKey = `${name}-expanded`;
  let isExpanded = true;
  if (localStorage.getItem(storageKey)) {
    isExpanded = localStorage.getItem(storageKey) == String(true);
  } else {
    isExpanded = true;
  }

  const toggle = () => {
    isExpanded = !isExpanded;
    if (name) {
      localStorage.setItem(storageKey, String(isExpanded));
    }
  };
</script>

<div class="section">
  <button
    class="heading"
    on:click={toggle}
    title={isExpanded ? "Collapse section" : "Expand section"}
    aria-expanded={isExpanded}
    aria-controls={`${name}-body`}
  >
    <h3>{title}</h3>
    <div class="arrow-container">
      <svg
        class={isExpanded ? "rotated" : ""}
        xmlns="http://www.w3.org/2000/svg"
        width="24"
        height="24"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        stroke-width="2"
        stroke-linecap="round"
        stroke-linejoin="round"><path d="M15 18l-6-6 6-6" /></svg
      >
    </div>
  </button>
  {#if isExpanded}
    <div id={`${name}-body`} class="body" transition:slide={{ duration: 300 }}>
      <slot />
    </div>
  {/if}
</div>

<style>
  .section {
    display: flex;
    flex-direction: column;
    background-color: var(--gray-90);
    width: 360px;
    height: 100%;
  }
  .heading {
    all: unset;
    background-color: var(--gray-85);
    padding: 0.8rem;
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: center;
    transition: background-color 0.2s;
  }
  .heading:hover {
    background-color: var(--gray-65);
  }
  .heading > h3 {
    font-size: 1.1rem;
  }

  .arrow-container {
    background-color: transparent;
    display: flex;
    align-items: center;
    padding: 0.2rem;
    border-radius: 0.5rem;
  }

  svg {
    transition: transform 0.3s;
    color: inherit;
  }
  svg.rotated {
    transform: rotate(-90deg);
  }

  .body {
    display: flex;
    flex-direction: column;
    padding: 1rem;
    gap: 1rem;
  }
</style>
