<script lang="ts">
  import { Sequence } from "../../../data/sequence";
  import { selectedSequence } from "../../../stores";

  export let sequence: Sequence;

  function select() {
    $selectedSequence = sequence;
  }
</script>

<button
  on:click={select}
  title="Edit {sequence.name} sequence"
  class="inverse"
  class:selected={$selectedSequence === sequence}
>
  {sequence.name}
</button>

<style>
  button {
    padding: 1rem;
    font-size: 1.3em;
    background-color: var(--gray-90);
    border-radius: 0.5em;
  }
  button.selected {
    background-color: var(--gray-75);
  }
</style>
