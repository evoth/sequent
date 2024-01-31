<script>
  import ThemeToggle from "./ui/ThemeToggle.svelte";
</script>

<main>
  <div>
    <h1>Sequent Editor</h1>
    <ThemeToggle />
  </div>
</main>

<style>
  div {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    column-gap: 8px;
  }

  main {
    display: flex;
    flex-direction: column;
    row-gap: 32px;
  }
</style>
