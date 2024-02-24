<script lang="ts">
  import { slide } from "svelte/transition";
  import Modal from "../Modal.svelte";

  export let isOpen: boolean;
  export let title: string;
  export let name = "";
  export let description = "";
  export let onSubmit = () => {};
  export let onClose = () => {};

  let showError = false;
  // Resets showError when opened
  $: showError = !isOpen;
</script>

<Modal bind:isOpen {title} {onSubmit} {onClose} canSubmit={name !== ""}>
  <div>
    <label>
      Name:
      <input
        bind:value={name}
        type="text"
        placeholder="Sequence name"
        required
        class:error={showError}
        on:blur={() => {
          name = name.trim();
          showError = name === "";
        }}
        on:input={() => (showError = name === "" && showError)}
      />
    </label>
    {#if showError}
      <p class="error" transition:slide={{ duration: 200 }}>
        Sequence name is required.
      </p>
    {/if}
  </div>
  <label>
    Description:
    <textarea
      bind:value={description}
      rows="6"
      placeholder="Sequence description"
    />
  </label>
</Modal>

<style>
  .error {
    margin-top: 0.5rem;
  }
</style>
