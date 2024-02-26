<script lang="ts">
  import ThemeToggle from "./ThemeToggle.svelte";
</script>

<div class="title">
  <h1>Sequent Editor</h1>
  <ThemeToggle />
</div>

<style>
  .title {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: center;
    column-gap: 8px;
    padding: 1.2rem;
    font-size: 0.8rem;
    background-color: var(--gray-95);
    border-bottom: var(--border-style);
  }
</style>
