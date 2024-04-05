<script>
  import ConnectEsp from "./lib/ConnectEsp.svelte";
  import Sequences from "./lib/Sequences.svelte";
  import Server from "./lib/Server.svelte";
  import Status from "./lib/Status.svelte";
  import ThemeToggle from "./lib/ThemeToggle.svelte";
  import { isConnected } from "./stores.js";
</script>

<main>
  <div>
    <h1>ESP Sequent Driver</h1>
    <ThemeToggle />
  </div>
  <Status />
  <ConnectEsp />
  {#if $isConnected}
    <Server />
    <Sequences />
  {/if}
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
