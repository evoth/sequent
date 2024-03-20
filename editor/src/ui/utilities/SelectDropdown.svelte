<script lang="ts">
  import Dropdown from "./Dropdown.svelte";

  export let label: string;
  export let buttonTitle: string | undefined;
  export let dropdownText: string;
  export let disabled = false;
</script>

<div class="select-dropdown">
  {label}:
  <Dropdown fullWidth lighter>
    <button
      slot="button"
      let:toggleDropdown
      on:click={toggleDropdown}
      title={buttonTitle}
      class="select-dropdown-button"
      {disabled}
    >
      {dropdownText}
      <svg
        xmlns="http://www.w3.org/2000/svg"
        width="1.3em"
        height="1.3em"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        stroke-width="2"
        stroke-linecap="round"
        stroke-linejoin="round"><path d="M6 9l6 6 6-6" /></svg
      >
    </button>
    <svelte:fragment slot="buttons" let:toggleDropdown>
      <slot name="buttons" {toggleDropdown} />
    </svelte:fragment>
  </Dropdown>
</div>

<style>
  .select-dropdown {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }
  .select-dropdown:has(button:disabled) {
    cursor: not-allowed;
    color: var(--gray-50);
  }

  .select-dropdown-button {
    background-color: var(--gray-85);
    padding: 0.6rem;
    display: flex;
    gap: 0.4rem;
    width: 100%;
    justify-content: space-between;
  }
</style>
