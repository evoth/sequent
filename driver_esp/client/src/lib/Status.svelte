<script>
  import { state } from "../stores.js";
  import Section from "./Section.svelte";
  import StatusHelper from "./StatusHelper.svelte";
</script>

<Section name="status">
  <h2 slot="heading">Status</h2>
  <StatusHelper title={"Server logs"} logs={$state.serverLogs} />
  <StatusHelper title={"Sequence logs"} logs={$state.sequenceLogs} />
  {#each $state.cameras as camera}
  <StatusHelper title={`Camera logs (${camera.method} @ ${camera.cameraIP})`} logs={camera.logs} />
  {/each}
</Section>
