<script>
  import { espIP, isConnected, isLoading, socket, state } from "../stores.js";
  import Section from "./Section.svelte";
  let espIPInput = $espIP,
    isConnecting;

  function connect() {
    $espIP = espIPInput;
    $socket?.close();
    $socket = new WebSocket(`ws://${$espIP}:81`);
    $socket.addEventListener("message", (event) => {
      $state = JSON.parse(event.data);
      console.log($state);
      $isLoading = false;
    });
    $socket.addEventListener("open", (event) => {
      $isConnected = true;
      isConnecting = false;
    });
    $socket.addEventListener("close", (event) => {
      $isConnected = false;
      isConnecting = false;
    });
    isConnecting = true;
  }

  function disconnect() {
    $socket?.close();
  }

  connect();
</script>

<Section name="connect-esp">
  <h2 slot="heading">Connect to ESP</h2>
  <label>
    <span>IP address:</span>
    <input bind:value={espIPInput} placeholder="ESP IP address" />
  </label>
  {#if !$isConnected || espIPInput != $espIP}
    <button on:click={connect}> Connect </button>
  {:else}
    <button on:click={disconnect}> Disconnect </button>
  {/if}
  {#if isConnecting}
    <p>Connecting...</p>
  {:else if $isConnected}
    <p class="success">Connected to {$espIP}</p>
  {:else}
    <p class="error">Disconnected</p>
  {/if}
</Section>
