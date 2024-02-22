<script lang="ts">
  import { slide } from "svelte/transition";
  import { Manager } from "../../../data/manager";
  import { Sequence } from "../../../data/sequence";
  import SequenceChip from "./SequenceChip.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";
  import { dndzone, type DndEvent, type Item } from "svelte-dnd-action";
  import { flip } from "svelte/animate";
  import { selectedSequence } from "../../../stores";

  export let manager: Manager<Sequence>;

  let sequences: SequenceItem[] = [...manager.children.entries()].map(
    ([id, sequence]) => {
      return { id, sequence };
    }
  );

  let modalOpen = false;
  let sequenceData = {
    name: "",
    description: "",
  };

  function newSequence() {
    const newSequence = new Sequence(
      manager,
      sequenceData.name,
      sequenceData.description
    );
    sequences = [
      ...sequences,
      {
        id: newSequence.id,
        sequence: newSequence,
      },
    ];
    $selectedSequence = newSequence;
  }

  function openNewSequenceModal() {
    sequenceData = { name: "", description: "" };
    modalOpen = true;
  }

  function handleDndConsider(e: CustomEvent<DndEvent>) {
    sequences = e.detail.items as SequenceItem[];
  }
  function handleDndFinalize(e: CustomEvent<DndEvent>) {
    sequences = e.detail.items as SequenceItem[];
  }
</script>

<div class="container">
  <section
    use:dndzone={{ items: sequences, dropTargetStyle: {}, dragAxis: "x" }}
    on:consider={handleDndConsider}
    on:finalize={handleDndFinalize}
  >
    {#each sequences as item (item.id)}
      <div animate:flip={{ duration: 200 }}>
        <SequenceChip sequence={item.sequence} bind:sequences />
      </div>
    {/each}
  </section>
  <button
    on:click={openNewSequenceModal}
    title="Add new sequence"
    style:font-size={sequences.length > 0 ? "1.6rem" : "1.3rem"}
  >
    {#if sequences.length === 0}
      + New sequence
    {:else}
      +
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
    padding: 0 1rem;
    column-gap: 0.8rem;
    border-bottom: 1px solid var(--gray-65);
  }

  section {
    display: flex;
    flex-direction: row;
    align-items: start;
    column-gap: 0.8rem;
  }

  button {
    border-radius: 0.8rem;
    padding: 0.5rem;
    margin-bottom: 0.6rem;
  }
  button:hover {
    background-color: var(--gray-65);
  }
</style>
