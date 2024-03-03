<script lang="ts">
  export let label: string;
  export let level = 0;
  export let offset: number;
</script>

{#if offset >= 0}
  <div class="container">
    <div
      class="tick"
      style:border-color={level > 0 ? "var(--gray-50)" : undefined}
      style:min-height={level > 0 ? "1rem" : undefined}
      style:height={level > 0 ? undefined : "100%"}
    >
      <div class="patch"></div>
    </div>
    <p class="label">{label}</p>
  </div>
{/if}

<style>
  .container {
    display: flex;
    align-items: center;
    width: 100%;
    height: 1.5rem;
    position: relative;
  }

  .patch {
    z-index: -1;
    background-color: var(--gray-95);
    position: absolute;
    left: -1px;
    width: 2px;
    height: 100%;
  }

  .tick {
    border-left: 1px solid var(--gray-35);
    position: relative;
  }

  .label {
    color: var(--gray-35);
    position: absolute;
    left: 0.4rem;
  }
</style>
