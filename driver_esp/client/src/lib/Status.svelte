<script>
  import { state } from "../stores.js";
  import Section from "./Section.svelte";
  import StatusHelper from "./StatusHelper.svelte";
</script>

<Section name="status">
  <h2 slot="heading">Status</h2>
  <StatusHelper name={"Server"} logs={$state.serverLogs} />
  <br>
  <StatusHelper name={"Sequence"} logs={$state.sequenceLogs} />
  <br>
  <StatusHelper name={"Camera"} logs={$state.cameraLogs} />
</Section>
