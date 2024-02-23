<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import { project } from "../../stores";
  import { clickoutside } from "@svelte-put/clickoutside";
  import { fade, slide } from "svelte/transition";
  import Modal from "../Modal.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";

  export let sequence: Sequence;
  export let sequences: SequenceItem[];

  let showOptions = false;
  let deleteModalOpen = false;
  let editModalOpen = false;
  // Disables showOptions when modal is opened
  $: showOptions = !deleteModalOpen && !editModalOpen && showOptions;
  let sequenceData = {
    name: "",
    description: "",
  };

  function openSequence() {
    $project.openedSequence = sequence;
  }

  function deleteSequence() {
    sequence.manager.children.delete(sequence.id);
    $project.sequenceManager.children = $project.sequenceManager.children;
    const index = sequences.findIndex((seq) => seq.sequence === sequence);
    sequences = sequences.filter((item) => item.sequence !== sequence);
    if ($project.openedSequence === sequence) {
      if (sequences.length > 0) {
        $project.openedSequence = sequences[Math.max(0, index - 1)].sequence;
      } else {
        $project.openedSequence = undefined;
      }
    }
  }

  function openEditModal() {
    sequenceData = {
      name: sequence.name ?? "",
      description: sequence.description ?? "",
    };
    editModalOpen = true;
  }

  function editSequence() {
    sequence.name = sequenceData.name;
    sequence.description = sequenceData.description;
    $project.sequenceManager.children = $project.sequenceManager.children;
  }
</script>

<div class="outer" class:opened={$project.openedSequence === sequence}>
  <button
    on:click={openSequence}
    title="Open {sequence.name} sequence"
    class="chip-button open"
  >
    <div>
      {sequence.name}
    </div>
  </button>
  <div
    class="options-container"
    use:clickoutside
    on:clickoutside={() => (showOptions = false)}
  >
    <button
      on:click={() => (showOptions = true)}
      title="Sequence options"
      class="chip-button options"
    >
      <div class="icon-container">
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
          ><circle cx="12" cy="12" r="1"></circle><circle cx="12" cy="5" r="1"
          ></circle><circle cx="12" cy="19" r="1"></circle></svg
        >
      </div>
    </button>
    {#if showOptions}
      <div class="options-menu" transition:fade={{ duration: 100 }}>
        <button on:click={openEditModal}>Edit</button>
        <button on:click={() => (deleteModalOpen = true)}>Delete</button>
      </div>
    {/if}
    <Modal
      bind:isOpen={deleteModalOpen}
      title="Delete sequence"
      onSubmit={deleteSequence}
    >
      <p>Delete the sequence named "{sequence.name}"?</p>
    </Modal>
    <SequenceEditModal
      title={"Edit sequence"}
      onSubmit={editSequence}
      bind:isOpen={editModalOpen}
      bind:name={sequenceData.name}
      bind:description={sequenceData.description}
    />
  </div>
</div>

<style>
  .outer {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0;
    font-size: 1.3rem;
    border-radius: 0.8rem;
    background-color: var(--gray-85);
    transition:
      background-color 0.2s,
      color 0.2s;
    white-space: nowrap;
    cursor: pointer;
    --outer-padding: 0.8rem;
    --bottom-padding: var(--outer-padding);
    --opened-bottom-padding: 1.3rem;
    position: relative;
    margin-bottom: calc(var(--opened-bottom-padding) - var(--bottom-padding));
  }
  .outer:not(:has(.options:hover)):not(.opened):hover {
    background-color: var(--gray-65);
  }
  .outer.opened {
    background-color: var(--gray-95);
    border: var(--border-style);
    --bottom-padding: var(--opened-bottom-padding);
    border-bottom-left-radius: 0;
    border-bottom-right-radius: 0;
    border-bottom-color: transparent;
  }

  .outer.opened::after {
    position: absolute;
    content: "";
    top: 100%;
    left: 0px;
    right: 0px;
    height: 2px;
    background-color: var(--gray-95);
  }

  .chip-button {
    font-size: 1.3rem;
    padding: var(--outer-padding);
    padding-bottom: var(--bottom-padding);
  }
  .chip-button:hover {
    background-color: transparent;
  }
  .open {
    padding-right: 0;
  }
  .options {
    padding: 0;
  }

  .icon-container {
    display: flex;
    align-items: center;
    padding: calc(var(--outer-padding) / 2) 0;
    margin: calc(var(--outer-padding) / 2) calc(var(--outer-padding) / 2)
      calc(var(--outer-padding) / 2) 0;
    border-radius: 0.5rem;
    transition:
      background-color 0.2s,
      color 0.2s;
  }
  .chip-button:hover > .icon-container {
    background-color: var(--gray-75);
  }

  .options-container {
    position: relative;
    z-index: 1;
  }

  .options-menu {
    position: absolute;
    top: 0.5rem;
    left: 0;
    display: flex;
    flex-direction: column;
    padding: 0.5rem;
    background-color: var(--gray-90);
    border-radius: 0.7rem;
    border: var(--border-style);
    box-shadow:
      0 10px 15px -3px var(--shadow),
      0 4px 6px -4px var(--shadow);
  }

  .options-menu > button {
    font-size: 1.2rem;
    padding: 0.3rem;
    text-align: start;
  }
</style>
