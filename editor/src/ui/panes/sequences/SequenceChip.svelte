<script lang="ts">
  import { Sequence } from "../../../data/sequence";
  import { selectedSequence } from "../../../stores";

  export let sequence: Sequence;

  function select() {
    $selectedSequence = sequence;
  }
</script>

<div class:selected={$selectedSequence === sequence}>
  <button
    on:click={select}
    title="Select {sequence.name} sequence"
    class="select inverse"
  >
    {sequence.name}
  </button>
  <button title="Sequence options" class="options inverse">
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
  </button>
</div>

<style>
  div {
    display: flex;
    align-items: center;
    gap: 0.5em;
    padding: 0;
    font-size: 1.3em;
    background-color: var(--gray-90);
    border-radius: 0.5em;
    transition:
      background-color 0.2s,
      color 0.2s;
  }
  div:not(:has(.options:hover)):hover {
    background-color: var(--gray-65);
  }
  div.selected {
    background-color: var(--gray-85);
    border: 1px solid var(--gray-35);
  }

  button {
    display: flex;
    align-items: center;
    padding: 1rem;
    background-color: transparent;
  }
  button:hover {
    background-color: transparent;
  }

  .select {
    padding-right: 0;
  }
  .options {
    padding: 0.5rem 0;
    margin: 0.5em 0.5em 0.5em 0;
  }
  .options:hover {
    background-color: var(--gray-65);
  }
</style>
