<script lang="ts">
  import { Manager } from "../../../data/manager";
  import { Sequence } from "../../../data/sequence";
  import SequenceChip from "./SequenceChip.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";

  export let manager: Manager<Sequence>;

  let modalOpen = false;
  let modalTitle = "";
  let onSubmit = () => {};
  let sequenceData = {
    name: "",
    description: "",
  };

  function submitNewSequenceModal() {
    new Sequence(manager, sequenceData.name, sequenceData.description);
    // I think I won't have to do this with Svelte 5
    manager.children = manager.children;
  }

  function openNewSequenceModal() {
    modalTitle = "New sequence";
    sequenceData = { name: "", description: "" };
    onSubmit = submitNewSequenceModal;
    modalOpen = true;
  }
</script>

<div>
  {#each manager.children as [id, sequence]}
    <SequenceChip {sequence} />
  {/each}
  <button
    on:click={openNewSequenceModal}
    title="Add new sequence"
    class="inverse">+</button
  >
  <SequenceEditModal
    title={modalTitle}
    {onSubmit}
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
    column-gap: 8px;
    padding: 1rem;
  }

  button {
    padding: 1rem;
    font-size: 1.3em;
    background-color: var(--gray-90);
    border-radius: 0.5em;
  }
</style>
