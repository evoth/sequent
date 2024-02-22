<script lang="ts">
  import { Modals, closeModal } from "svelte-modals";
  import ThemeToggle from "./ui/ThemeToggle.svelte";
  import Sequences from "./ui/panes/sequences/Sequences.svelte";
  import Timeline from "./ui/panes/timeline/Timeline.svelte";
  import { Manager } from "./data/manager";
  import { Sequence } from "./data/sequence";
  import { modalOpen } from "./stores";

  const sequenceManager = new Manager<Sequence>();
</script>

<main>
  <div>
    <h1>Sequent Editor</h1>
    <ThemeToggle />
  </div>
  <Sequences manager={sequenceManager} />
  <Timeline />
</main>

<style>
  div {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: center;
    column-gap: 8px;
    padding: 1.2rem;
    font-size: 0.8rem;
  }

  main {
    height: 100%;
  }
</style>
