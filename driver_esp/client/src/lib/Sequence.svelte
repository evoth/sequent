<script>
  import { espIP, isLoading, socket, state } from "../stores.js";
  import Section from "./Section.svelte";
  let isUpdating = "";

  let timeReceived = {};
  let timeUntilNext = {};
  let interval = {};
  let seqFiles = [];
  let seqStates = {};
  refreshList();

  state.subscribe((value) => {
    console.log(value);
    seqStates = getStates(seqFiles);
    for (const [sequencePath, sequenceState] of Object.entries(seqStates)) {
      timeReceived[sequencePath] = Date.now();
      clearInterval(interval[sequencePath]);
      if (sequenceState.isRunning) {
        interval[sequencePath] = setInterval(() => {
          timeUntilNext[sequencePath] = (
            (sequenceState.timeUntilNext -
              (Date.now() - timeReceived[sequencePath])) /
            1000
          ).toFixed(1);
        }, 100);
      } else {
        timeUntilNext[sequencePath] = "0";
      }
    }
  });

  isLoading.subscribe(
    (value) => (isUpdating = value && isUpdating !== "" ? isUpdating : "")
  );

  function start(filename) {
    $socket.send(JSON.stringify({ command: "start", filename }));
    $isLoading = true;
    isUpdating = filename;
  }

  function stop(filename) {
    $socket.send(JSON.stringify({ command: "stop", filename }));
    $isLoading = true;
    isUpdating = filename;
  }

  async function refreshList() {
    const response = await fetch(`http://${$espIP}/seq-files`);
    seqFiles = (await response.json()).files;
    seqStates = getStates(seqFiles);
    console.log(seqStates);
  }

  async function deleteSequence(filename) {
    await fetch(
      `http://${$espIP}/delete?file=` + encodeURIComponent(filename),
      { method: "DELETE" }
    );
    refreshList();
  }

  function getStates(filenames) {
    return Object.fromEntries(
      filenames.map((filename) => [
        filename,
        $state.states.find((state) => state.filePath === filename) ?? {},
      ])
    );
  }
</script>

<Section name="sequence">
  <h2 slot="heading">Sequence</h2>

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

  {#each seqFiles as filename}
    <div class="sequence-container">
      <div class="sequence-container-inner">
        <div>{filename}</div>
        <div style="flex: 1"></div>
        {#if seqStates[filename].isRunning}
          <button
            title={`Stop '${filename}' sequence`}
            on:click={() => stop(filename)}
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
              ><rect x="3" y="3" width="18" height="18" rx="2" ry="2"
              ></rect></svg
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

      <div class="status">
        {#if Object.keys(seqStates[filename]).length > 0}
          {#if isUpdating === filename}
            <p>Loading...</p>
          {:else if seqStates[filename].isRunning}
            <p>Sequence running...</p>
          {:else}
            <p>Sequence stopped.</p>
          {/if}

          {#if seqStates[filename].isRunning}
            <p>
              Action index: {seqStates[filename].actionIndex} / {seqStates[
                filename
              ].totalActions}
              Time until next action: {timeUntilNext[filename]}
            </p>
          {/if}
        {/if}
      </div>
    </div>
  {/each}
</Section>

<style>
  .sequence-container {
    border-radius: 8px;
    padding: 1em;
    color: var(--gray-5);
    background-color: var(--gray-90);
    transition:
      background-color 0.2s,
      color 0.2s;
    width: 100%;
  }

  .sequence-container-inner {
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .status {
    color: var(--gray-35);
  }
</style>
