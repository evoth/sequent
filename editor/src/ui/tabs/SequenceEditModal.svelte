<script context="module" lang="ts">
  export type SequenceData = {
    name: string;
    description: string;
    isAbsolute?: boolean;
    hue: number;
  };
</script>

<script lang="ts">
  import { slide } from "svelte/transition";
  import ColorSlider from "../utilities/ColorSlider.svelte";
  import Modal from "../utilities/Modal.svelte";

  export let isOpen: boolean;
  export let title: string;
  export let data: SequenceData;
  export let onSubmit = () => {};
  export let onClose = () => {};
  export let isNew = false;

  let showError = false;
  // Resets showError when opened
  $: showError = !isOpen;

  let nameInput: HTMLElement;
  $: nameInput && isOpen && nameInput.focus();
</script>

<Modal bind:isOpen {title} {onSubmit} {onClose} canSubmit={data.name !== ""}>
  <div>
    <label>
      Name:
      <input
        bind:this={nameInput}
        bind:value={data.name}
        type="text"
        placeholder="Sequence name"
        required
        class:error={showError}
        on:blur={() => {
          data.name = data.name.trim();
          showError = data.name === "";
        }}
        on:input={() => (showError = data.name === "" && showError)}
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
      bind:value={data.description}
      rows="6"
      placeholder="Sequence description"
    />
  </label>
  {#if isNew}
    <label class="horizontal">
      <input type="checkbox" bind:checked={data.isAbsolute} />
      Use absolute time?
    </label>
  {/if}
  <div class="color-input">
    <div>Color:</div>
    <ColorSlider bind:hue={data.hue} />
  </div>
</Modal>

<style>
  .error {
    margin-top: 0.5rem;
  }

  .color-input {
    display: flex;
    flex-direction: column;
    gap: 0.5rem;
  }
</style>
