<script lang="ts">
  import ThemeToggle from "./ui/ThemeToggle.svelte";
  import SequenceTabs from "./ui/tabs/SequenceTabs.svelte";
  import Timeline from "./ui/timeline/Timeline.svelte";
  import Pane from "./ui/panes/Pane.svelte";
  import PaneSection from "./ui/panes/PaneSection.svelte";
  import ComponentsSection from "./ui/panes/components/ComponentsSection.svelte";
  import { project } from "./stores";
  import { Manager } from "./data/manager";
  import type { Action } from "./data/action";
  import { Parameter } from "./data/parameter";
  import type { Sequence } from "./data/sequence";
  import type { Timestamp } from "./data/timestamp";
  import { ActionSet } from "./data/actionSet";
  import type { CustomJSON } from "./data/serialization";

  const json = JSON.parse(
    JSON.stringify($project.actionSet, (key, value) =>
      value instanceof Map ? Object.fromEntries(value) : value
    )
  );
  console.log(json);
  const managers = {
    actionManager: new Manager<Action>(),
    parameterManager: new Manager<Parameter<any>>(),
    sequenceManager: new Manager<Sequence>(),
    timestampManager: new Manager<Timestamp>(),
  };
  console.log(ActionSet.fromJSON(json, managers));
</script>

<main>
  <div class="title">
    <h1>Sequent Editor</h1>
    <ThemeToggle />
  </div>
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
