<script>
  import { state, isLoading, socket, espIP } from "../stores.js";
  import Section from "./Section.svelte";
  let intervalSec, isUpdating, bulbSec;

let timeReceived;
let timeUntilNext = "0";
let interval;
let seqFiles = [];

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
  };

  function stop() {
    $socket.send(JSON.stringify({ command: "stop" }));
    isUpdating = $isLoading = true;
  };

  async function refreshList() {
    const response = await fetch(`http://${$espIP}/seq-files`);
    seqFiles = (await response.json()).files;
  }
</script>

<Section name="sequence">
  <h2 slot="heading">Sequence</h2>
  
  <button on:click={refreshList}> Refresh </button>

  {#each seqFiles as filename}
  <button on:click={() => $state.isRunning && $state.sequenceFilename === filename ? stop() : start(filename)}>{filename.slice(1)}</button>
  {/each}

  {#if isUpdating}
    <p>Loading...</p>
  {:else if $state.isRunning}
    <p>Sequence running...</p>
  {:else}
    <p>Sequence stopped.</p>
  {/if}

  {#if $state.isRunning}
    <p>
      Action index: {$state.actionIndex} / {$state.totalActions}
      Time until next action: {timeUntilNext}
    </p>
  {/if}
</Section>
