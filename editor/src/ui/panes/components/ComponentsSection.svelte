<script lang="ts">
  import { Action } from "../../../data/action";
  import { Manager } from "../../../data/manager";
  import { Sequence } from "../../../data/sequence";
  import PaneSection from "../PaneSection.svelte";

  export let title: string;
  export let name: string;
  export let manager: Manager<Action | Sequence>;
</script>

<PaneSection {title} {name}>
  {#each manager.children.entries() as [id, component] (id)}
    <div
      class="component"
      style:background-color={`hsl(${component.hue}deg var(--component-saturation) var(--component-lightness))`}
    >
      {component.name}
    </div>
  {/each}
</PaneSection>

<style>
  .component {
    padding: 1rem;
    border-radius: 1rem;
    font-weight: 500;
  }
</style>
