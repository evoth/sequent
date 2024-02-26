<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import { project } from "../../stores";
  import SequenceChip from "./SequenceChip.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";

  let modalOpen = false;
  let sequenceData = {
    name: "",
    description: "",
  };

  function newSequence() {
    const newSequence = new Sequence(
      $project.sequenceManager,
      sequenceData.name,
      sequenceData.description
    );
    $project.sequenceManager.children = $project.sequenceManager.children;
    $project.openedSequence = newSequence;
  }

  function openNewSequenceModal() {
    sequenceData = { name: "", description: "" };
    modalOpen = true;
  }
</script>

<div class="container">
  {#each $project.sequenceManager.children.values() as sequence (sequence.id)}
    <SequenceChip {sequence} />
  {/each}
  <button
    on:click={openNewSequenceModal}
    title="Add new sequence"
    style:font-size={$project.sequenceManager.children.size > 0
      ? "1.6rem"
      : "1.3rem"}
  >
    {#if $project.sequenceManager.children.size === 0}
      + New sequence
    {:else}
      <svg
        xmlns="http://www.w3.org/2000/svg"
        width="24"
        height="24"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        stroke-width="2"
        stroke-linecap="round"
        stroke-linejoin="round"
        ><line x1="12" y1="5" x2="12" y2="19"></line><line
          x1="5"
          y1="12"
          x2="19"
          y2="12"
        ></line></svg
      >
    {/if}
  </button>
  <SequenceEditModal
    title={"New sequence"}
    onSubmit={newSequence}
    bind:isOpen={modalOpen}
    bind:name={sequenceData.name}
    bind:description={sequenceData.description}
  />
</div>

<style>
  .container {
    display: flex;
    padding: 0rem 1rem;
    border-bottom: var(--border-style);
  }

  button {
    border-radius: 0.8rem;
    padding: 0.5rem;
    margin-bottom: 0.6rem;
    display: flex;
    flex-direction: column;
    justify-content: center;
  }
  button:hover {
    background-color: var(--gray-65);
  }
</style>
