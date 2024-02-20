<script lang="ts">
  import { fade } from "svelte/transition";
  import { scale } from "svelte/transition";
  import { cubicInOut } from "svelte/easing";
  export let isOpen: Boolean;
  export let title: string;

  export let onClose = () => {};
  export let onSubmit = () => {};

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
        <button on:click={close}>Cancel</button>
        <button on:click={submit}>OK</button>
      </div>
    </div>
  </div>
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
    min-width: 360px;
    border-radius: 1em;
    padding: 1.5em;
    background: var(--gray-90);
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    pointer-events: auto;
  }

  .contents {
    padding: 1em 0em 1.5em 0em;
    display: flex;
    flex-direction: column;
    gap: 1em;
  }

  h2 {
    text-align: center;
  }

  .actions {
    display: flex;
    justify-content: flex-end;
    gap: 0.5em;
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
