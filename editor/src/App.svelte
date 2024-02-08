<script lang="ts">
  import SequenceTest from "./ui/SequenceTest.svelte";
  import ThemeToggle from "./ui/ThemeToggle.svelte";
</script>

<main>
  <div>
    <h1>Sequent Editor</h1>
    <ThemeToggle />
  </div>
  <SequenceTest />
</main>

<style>
  div {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    column-gap: 8px;
  }

  main {
    height: 100%;
  }
</style>
