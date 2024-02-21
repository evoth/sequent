<script lang="ts">
  import { fade } from "svelte/transition";
  import Portal from "svelte-portal";
  export let isOpen: boolean;
  export let title: string;

  export let onClose = () => {};
  export let onSubmit = () => {};
  export let canSubmit = true;

  function close() {
    isOpen = false;
    onClose();
  }

  function submit() {
    isOpen = false;
    onSubmit();
  }
</script>

{#if isOpen}
  <!-- TODO: find a better way? -->
  <!-- svelte-ignore a11y-click-events-have-key-events -->
  <!-- svelte-ignore a11y-no-static-element-interactions -->
  <Portal target="body">
    <div
      on:click={close}
      class="backdrop"
      in:fade={{ duration: 200 }}
      out:fade={{ duration: 100 }}
    ></div>
    <div
      role="dialog"
      class="container"
      in:fade={{ duration: 200 }}
      out:fade={{ duration: 100 }}
    >
      <div class="modal">
        <h2>{title}</h2>
        <div class="contents">
          <slot />
        </div>
        <div class="actions">
          <slot name="actions" />
          <button on:click={close} class="inverse">Cancel</button>
          <button on:click={submit} disabled={!canSubmit} class="inverse"
            >OK</button
          >
        </div>
      </div>
    </div>
  </Portal>
{/if}

<style>
  .container {
    position: fixed;
    top: 0;
    bottom: 0;
    right: 0;
    left: 0;
    display: flex;
    justify-content: center;
    align-items: center;

    /* allow click-through to backdrop */
    pointer-events: none;
  }

  .modal {
    width: 400px;
    border-radius: 1rem;
    padding: 1.5rem;
    background: var(--gray-90);
    pointer-events: auto;
    margin: 2rem;
  }

  .contents {
    padding: 1rem 0 1.5rem 0;
    display: flex;
    flex-direction: column;
    gap: 1rem;
  }

  h2 {
    text-align: center;
    font-size: 1.5rem;
  }

  .actions {
    display: flex;
    justify-content: flex-end;
    gap: 0.5rem;
  }

  .backdrop {
    position: fixed;
    top: 0;
    bottom: 0;
    right: 0;
    left: 0;
    background: rgba(0, 0, 0, 0.25);
  }
</style>
