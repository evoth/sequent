<script>
  import { state, isLoading, socket } from "../stores.js";
  import Section from "./Section.svelte";
  let intervalSec, isUpdating, bulbSec;

let timeReceived;
let timeUntilNext = "0";
let interval;
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

  const start = () => {
    $socket.send(JSON.stringify({ command: "start" }));
    isUpdating = $isLoading = true;
  };

  const stop = () => {
    $socket.send(JSON.stringify({ command: "stop" }));
    isUpdating = $isLoading = true;
  };
</script>

<Section name="sequence">
  <h2 slot="heading">Sequence</h2>
  
  {#if $state.isRunning}
    <button on:click={stop}> Stop </button>
  {:else}
    <!-- TODO: Add some basic validation -->
    <button on:click={start}> Start </button>
  {/if}

  {#if isUpdating}
    <p>Loading...</p>
  {:else if $state.isRunning}
    <p>Sequence running...</p>
  {:else}
    <p>Sequence stopped.</p>
  {/if}

  {#if $state.cameraConnected}
    <p>
      Action index: {$state.actionIndex} / {$state.totalActions}
      Time until next action: {timeUntilNext}
    </p>
  {/if}
</Section>
