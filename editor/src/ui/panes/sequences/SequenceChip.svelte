<script lang="ts">
  import { Sequence } from "../../../data/sequence";
  import { selectedSequence } from "../../../stores";
  import { clickoutside } from "@svelte-put/clickoutside";
  import { fade, slide } from "svelte/transition";
  import { cubicOut, cubicIn } from "svelte/easing";
  import Modal from "../../Modal.svelte";
  import type { Manager } from "../../../data/manager";
  import SequenceEditModal from "./SequenceEditModal.svelte";

  export let sequence: Sequence;
  export let manager: Manager<Sequence>;

  let showOptions = false;
  let deleteModalOpen = false;
  let editModalOpen = false;
  // Disables showOptions when modal is opened
  $: showOptions = !deleteModalOpen && !editModalOpen && showOptions;
  let sequenceData = {
    name: "",
    description: "",
  };

  function selectSequence() {
    $selectedSequence = sequence;
  }

  function deleteSequence() {
    if ($selectedSequence === sequence) {
      $selectedSequence = undefined;
    }
    manager.children.delete(sequence.id);
    manager.children = manager.children;
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
  }
</script>

<div
  class="outer"
  class:selected={$selectedSequence === sequence}
  transition:slide={{ duration: 200, axis: "x" }}
>
  <button
    on:click={selectSequence}
    title="Select {sequence.name} sequence"
    class="chip-button select"
  >
    {sequence.name}
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
      <div>
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
    background-color: var(--gray-90);
    border-radius: 0.8rem;
    transition:
      background-color 0.2s,
      color 0.2s;
    white-space: nowrap;
  }
  .outer:not(:has(.options:hover)):hover {
    background-color: var(--gray-65);
  }
  .outer.selected {
    background-color: var(--gray-85);
    border: 1px solid var(--gray-35);
  }

  .chip-button {
    font-size: 1.3rem;
    padding: 1rem;
  }
  .chip-button:hover {
    background-color: transparent;
  }

  .select {
    padding-right: 0;
  }
  .options {
    padding: 0;
  }

  .chip-button > div {
    display: flex;
    align-items: center;
    padding: 0.5rem 0;
    margin: 0.5rem 0.5rem 0.5rem 0;
    border-radius: 0.5rem;
    transition:
      background-color 0.2s,
      color 0.2s;
  }
  .chip-button:hover > div {
    background-color: var(--gray-65);
  }

  .options-container {
    position: relative;
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
    border: 1px solid var(--gray-50);
    box-shadow:
      0 10px 15px -3px rgb(0 0 0 / 0.6),
      0 4px 6px -4px rgb(0 0 0 / 0.6);
  }

  .options-menu > button {
    font-size: 1.2rem;
    padding: 0.3rem;
    text-align: start;
  }
</style>
