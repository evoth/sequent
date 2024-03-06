<script lang="ts">
  import { project } from "./data/stores";
  import Pane from "./ui/panes/Pane.svelte";
  import PaneSection from "./ui/panes/PaneSection.svelte";
  import ComponentsSection from "./ui/panes/components/ComponentsSection.svelte";
  import SequenceTabs from "./ui/tabs/SequenceTabs.svelte";
  import Timeline from "./ui/timeline/Timeline.svelte";
  import TitleBar from "./ui/titleBar/TitleBar.svelte";
</script>

<main>
  <TitleBar />
  <div class="panes">
    <Pane title="New components">
      <ComponentsSection
        title="Actions"
        name="components-actions"
        bind:manager={$project.actionSet.actionManager}
      />
      <ComponentsSection
        title="Sequences"
        name="components-sequences"
        bind:manager={$project.sequenceManager}
      />
    </Pane>
    <div class="timeline">
      <SequenceTabs />
      {#if $project.openedSequence !== undefined}
        <Timeline sequence={$project.openedSequence} />
      {/if}
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
    display: flex;
    flex-direction: column;
  }

  .panes {
    flex: 1;
    display: flex;
  }

  .timeline {
    padding-top: 1rem;
    flex: 1;
    border-left: var(--border-style);
    border-right: var(--border-style);
    display: flex;
    flex-direction: column;
  }
</style>
