<script>
  import { espIP, socket } from "../stores.js";
  import Section from "./Section.svelte";

  let fileInput;

  function setTime() {
    $socket.send(
      JSON.stringify({ command: "setTime", time: new Date().getTime() })
    );
  }

  function reboot() {
    fetch(`http://${$espIP}/reboot`, { method: "POST" });
  }

  function onFileChange() {
    fileInput = fileInput;
  }
</script>

<Section name="server">
  <h2 slot="heading">Server</h2>

  <button on:click={setTime}> Set time from client </button>
  <button on:click={reboot}> Reboot ESP </button>
  <form
    class="file-form"
    action={`http://${$espIP}/upload`}
    method="post"
    enctype="multipart/form-data"
  >
    <label class="button">
      Choose sequence file
      <input
        bind:this={fileInput}
        on:change={onFileChange}
        type="file"
        name="file"
        accept=".seq"
      />
    </label>
    {#if fileInput && fileInput.files.length > 0}
      <input
        class="button"
        type="submit"
        value={`Upload '${fileInput.files[0].name}'`}
      />
    {/if}
  </form>
</Section>

<style>
  input[type="file"] {
    display: none;
  }

  .file-form {
    display: flex;
    flex-direction: column;
    row-gap: 16px;
  }
</style>
