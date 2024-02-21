<script lang="ts">
  import { Manager } from "../../../data/manager";
  import { Sequence } from "../../../data/sequence";
  import SequenceChip from "./SequenceChip.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";

  export let manager: Manager<Sequence>;

  let modalOpen = false;
  let sequenceData = {
    name: "",
    description: "",
  };

  function newSequence() {
    new Sequence(manager, sequenceData.name, sequenceData.description);
    // I think I won't have to do this with Svelte 5
    manager.children = manager.children;
  }

  function openNewSequenceModal() {
    sequenceData = { name: "", description: "" };
    modalOpen = true;
  }
</script>

<div>
  {#each manager.children as [id, sequence] (sequence.id)}
    <SequenceChip {sequence} bind:manager />
  {/each}
  <button on:click={openNewSequenceModal} title="Add new sequence">+</button>
  <SequenceEditModal
    title={"New sequence"}
    onSubmit={newSequence}
    bind:isOpen={modalOpen}
    bind:name={sequenceData.name}
    bind:description={sequenceData.description}
  />
</div>

<style>
  div {
    display: flex;
    flex-direction: row;
    flex-wrap: wrap;
    align-items: center;
    column-gap: 0.8rem;
    row-gap: 0.8rem;
    padding: 1rem;
  }

  button {
    padding: 1rem;
    font-size: 1.3rem;
    background-color: var(--gray-90);
    border-radius: 0.8rem;
  }
  button:hover {
    background-color: var(--gray-65);
  }
</style>
