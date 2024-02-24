<script lang="ts">
  import { Sequence } from "../../data/sequence";
  import SequenceChip from "./SequenceChip.svelte";
  import SequenceEditModal from "./SequenceEditModal.svelte";
  import { dndzone, type DndEvent } from "svelte-dnd-action";
  import { flip } from "svelte/animate";
  import { project } from "../../stores";
  import { Project } from "../../data/project";

  let sequences: SequenceItem[] = [
    ...$project.sequenceManager.children.entries(),
  ].map(([id, sequence]) => {
    return { id, sequence };
  });

  let modalOpen = false;
  let sequenceData = {
    name: "",
    description: "",
  };

  // $: sequences,
  //   (() => {
  //     let json = JSON.parse(
  //       JSON.stringify($project, (key, value) =>
  //         value instanceof Map ? Object.fromEntries(value) : value
  //       )
  //     );
  //     console.log(json);
  //     console.log(Project.fromJSON(json));
  //   })();

  function newSequence() {
    const newSequence = new Sequence(
      $project.sequenceManager,
      sequenceData.name,
      sequenceData.description
    );
    $project.sequenceManager.children = $project.sequenceManager.children;
    sequences = [
      ...sequences,
      {
        id: newSequence.id,
        sequence: newSequence,
      },
    ];
    $project.openedSequence = newSequence;
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
    border-bottom: var(--border-style);
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
