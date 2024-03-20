<script lang="ts">
  import { project } from "../../data/stores";
  import SequenceTabs from "../tabs/SequenceTabs.svelte";
  import Timeline from "../timeline/Timeline.svelte";
  import { BREAKPOINT_LG, BREAKPOINT_XL } from "../utilities/breakpoints";
  import Pane from "./Pane.svelte";
  import ComponentsSection from "./components/ComponentsSection.svelte";
  import ParametersSection from "./properties/ParametersSection.svelte";
  import RepetitionSection from "./properties/RepetitionSection.svelte";

  export let width: number;

  let timelineWidth = 0;
</script>

<div class="panes" style:height={width < BREAKPOINT_LG ? "60vh" : undefined}>
  {#if width >= BREAKPOINT_LG}
    <div class="leftPanes">
      <Pane>
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
        {#if width < BREAKPOINT_XL}
          <ParametersSection />
          <RepetitionSection />
        {/if}
      </Pane>
    </div>
  {/if}
  <div class="timeline">
    <SequenceTabs {timelineWidth} />
    {#if $project.openedSequence !== undefined}
      <Timeline
        bind:sequence={$project.openedSequence}
        bind:width={timelineWidth}
      />
    {/if}
  </div>
  {#if width >= BREAKPOINT_XL}
    <Pane>
      <ParametersSection />
      <RepetitionSection />
    </Pane>
  {/if}
</div>
{#if width < BREAKPOINT_LG}
  <div>
    <Pane fullWidth>
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
      <ParametersSection />
      <RepetitionSection />
    </Pane>
  </div>
{/if}

<style>
  .panes {
    flex: 1;
    display: flex;
    min-height: 0;
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
