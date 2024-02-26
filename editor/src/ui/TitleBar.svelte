<script lang="ts">
  import Dropdown from "./Dropdown.svelte";
  import ThemeToggle from "./ThemeToggle.svelte";
</script>

<div class="title">
  <h1>Sequent Editor</h1>
  <Dropdown>
    <button
      class="menu-button"
      slot="button"
      let:openDropdown
      on:click={openDropdown}
      title="File options">File</button
    >
    <svelte:fragment slot="buttons">
      <button>Open</button>
      <button>Export</button>
    </svelte:fragment>
  </Dropdown>
  <div class="theme-button">
    <ThemeToggle />
  </div>
</div>

<style>
  h1 {
    padding-left: 0.5rem;
    padding-right: 1.5rem;
  }

  .title {
    display: flex;
    flex-direction: row;
    align-items: center;
    padding: 1rem;
    font-size: 0.8rem;
    background-color: var(--gray-95);
    border-bottom: var(--border-style);
  }

  .menu-button {
    padding: 0.7rem;
    font-size: 1.3rem;
  }

  .theme-button {
    flex: 1;
    display: flex;
    justify-content: end;
  }
</style>
