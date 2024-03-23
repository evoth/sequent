<script>
  import { state } from "../stores.js";
  import Section from "./Section.svelte";
  import StatusHelper from "./StatusHelper.svelte";
</script>

<Section name="status">
  <h2 slot="heading">Status</h2>
  {#each $state.states as stateData}
  <StatusHelper title={stateData.name} logs={stateData.logs} />
  {/each}
</Section>
