<script lang="ts">
  import { Manager } from "../../../data/manager";
  import { Sequence } from "../../../data/sequence";
  import SequenceChip from "./SequenceChip.svelte";
  import Modal from "../../Modal.svelte";

  export let manager: Manager<Sequence>;

  let newModalOpen = false;
</script>

<div>
  {#each manager.children.values() as sequence}
    <SequenceChip {sequence} />
  {/each}
  <button
    on:click={() => (newModalOpen = true)}
    title="Add new sequence"
    class="inverse">+</button
  >
  <Modal bind:isOpen={newModalOpen} title={"New sequence"}>
    <label>
      Name:
      <input type="text" placeholder="Sequence name" />
    </label>
    <label>
      Description:
      <textarea rows="6" placeholder="Sequence description" />
    </label>
  </Modal>
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
