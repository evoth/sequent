<script lang="ts">
  import ThemeToggle from "./ui/ThemeToggle.svelte";
  import Sequences from "./ui/tabs/SequenceTabs.svelte";
  import Timeline from "./ui/timeline/Timeline.svelte";
  import { Manager } from "./data/manager";
  import { Sequence } from "./data/sequence";
  import Pane from "./ui/panes/Pane.svelte";
  import PaneSection from "./ui/panes/PaneSection.svelte";

  const sequenceManager = new Manager<Sequence>();
</script>

<main>
  <div class="title">
    <h1>Sequent Editor</h1>
    <ThemeToggle />
  </div>
  <div class="panes">
    <Pane title="New components">
      <PaneSection title="Actions" name="components-actions" />
      <PaneSection title="Sequences" name="components-sequences" />
    </Pane>
    <div class="timeline">
      <Sequences manager={sequenceManager} />
      <Timeline />
    </div>
    <Pane title="Properties">
      <PaneSection title="Parameters" name="properties-parameters" />
      <PaneSection title="Repitition" name="properties-repitition" />
    </Pane>
  </div>
</main>

<style>
  main {
    height: 100%;
    --border-style: 1px solid var(--gray-65);
    --border-style-dim: 1px solid var(--gray-85);
  }

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

  .panes {
    display: flex;
    height: 100%;
  }

  .timeline {
    padding-top: 1rem;
    flex: 1;
    border-left: var(--border-style);
    border-right: var(--border-style);
  }
</style>
