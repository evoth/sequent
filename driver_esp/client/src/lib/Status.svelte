<script>
  import { state } from "../stores.js";
  import Section from "./Section.svelte";
</script>

<Section name="status">
  <h2 slot="heading">Status</h2>
  <p
    class:success={$state.statusCode == 200}
    class:warning={$state.statusCode == 0}
    class:error={$state.statusCode != 200 && $state.statusCode != 0}
  >
    Status code: {$state.statusCode}
    Status message: {$state.statusMsg}
  </p>
</Section>
