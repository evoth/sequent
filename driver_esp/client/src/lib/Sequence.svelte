<script>
  import { espIP, isLoading, socket, state } from "../stores.js";
  import Section from "./Section.svelte";
  let isUpdating;

  let timeReceived;
  let timeUntilNext = "0";
  let interval;
  let seqFiles = [];
  refreshList();

  state.subscribe((value) => {
    timeReceived = Date.now();
    clearInterval(interval);
    if (value.isRunning) {
      interval = setInterval(() => {
        timeUntilNext = (
          ($state.timeUntilNext - (Date.now() - timeReceived)) /
          1000
        ).toFixed(1);
      }, 100);
    } else {
      timeUntilNext = "0";
    }
  });

  isLoading.subscribe((value) => (isUpdating = value && isUpdating));

  function start(filename) {
    $socket.send(JSON.stringify({ command: "start", filename }));
    isUpdating = $isLoading = true;
  }

  function stop() {
    $socket.send(JSON.stringify({ command: "stop" }));
    isUpdating = $isLoading = true;
  }

  async function refreshList() {
    const response = await fetch(`http://${$espIP}/seq-files`);
    seqFiles = (await response.json()).files;
  }

  async function deleteSequence(filename) {
    await fetch(
      `http://${$espIP}/delete?file=` + encodeURIComponent(filename),
      { method: "DELETE" }
    );
    refreshList();
  }
</script>

<Section name="sequence">
  <h2 slot="heading">Sequence</h2>

  <div class="icon-buttons-container">
    {#if $state.isRunning}
      <button on:click={stop} title="Stop running sequence">
        <svg
          xmlns="http://www.w3.org/2000/svg"
          width="1em"
          height="1em"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          stroke-width="2"
          stroke-linecap="round"
          stroke-linejoin="round"
          ><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect></svg
        >
      </button>
    {/if}

    <button on:click={refreshList} title="Refresh sequence list">
      <svg
        xmlns="http://www.w3.org/2000/svg"
        width="1em"
        height="1em"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        stroke-width="2"
        stroke-linecap="round"
        stroke-linejoin="round"
        ><path d="M21.5 2v6h-6M21.34 15.57a10 10 0 1 1-.57-8.38" /></svg
      >
    </button>
  </div>

  {#if isUpdating}
    <p>Loading...</p>
  {:else if $state.isRunning}
    <p>Sequence '{$state.sequenceFilename}' running...</p>
  {:else}
    <p>Sequence stopped.</p>
  {/if}

  {#if $state.isRunning}
    <p>
      Action index: {$state.actionIndex} / {$state.totalActions}
      Time until next action: {timeUntilNext}
    </p>
  {/if}

  {#each seqFiles as filename}
    <div class="sequence-container">
      <div>{filename}</div>
      <div style="flex: 1"></div>
      {#if $state.isRunning && $state.sequenceFilename === filename}
        <button title={`Stop '${filename}' sequence`} on:click={() => stop()}
          ><svg
            xmlns="http://www.w3.org/2000/svg"
            width="1em"
            height="1em"
            viewBox="0 0 24 24"
            fill="none"
            stroke="currentColor"
            stroke-width="2"
            stroke-linecap="round"
            stroke-linejoin="round"
            ><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect></svg
          ></button
        >
      {:else}
        <button
          title={`Start '${filename}' sequence`}
          on:click={() => start(filename)}
          ><svg
            xmlns="http://www.w3.org/2000/svg"
            width="1em"
            height="1em"
            viewBox="0 0 24 24"
            fill="none"
            stroke="currentColor"
            stroke-width="2"
            stroke-linecap="round"
            stroke-linejoin="round"
            ><polygon points="5 3 19 12 5 21 5 3"></polygon></svg
          ></button
        >
      {/if}
      <button
        title={`Delete '${filename}' sequence`}
        on:click={() => deleteSequence(filename)}
        ><svg
          xmlns="http://www.w3.org/2000/svg"
          width="1em"
          height="1em"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          stroke-width="2"
          stroke-linecap="round"
          stroke-linejoin="round"
          ><polyline points="3 6 5 6 21 6"></polyline><path
            d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"
          ></path><line x1="10" y1="11" x2="10" y2="17"></line><line
            x1="14"
            y1="11"
            x2="14"
            y2="17"
          ></line></svg
        ></button
      >
    </div>
  {/each}
</Section>

<style>
  .icon-buttons-container {
    display: flex;
    gap: 8px;
  }
  .icon-buttons-container > button {
    flex: 1;
  }

  .sequence-container {
    display: flex;
    align-items: center;
    gap: 8px;
    border-radius: 8px;
    padding: 0.6em 0.6em 0.6em 1em;
    color: var(--gray-5);
    background-color: var(--gray-90);
    transition:
      background-color 0.2s,
      color 0.2s;
    width: 100%;
  }
</style>
