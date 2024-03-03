<script lang="ts">
  export let label: string;
  export let level = 0;
</script>

<div class="container">
  <div
    class="tick"
    style:border-color={level > 0 ? "var(--gray-50)" : undefined}
    style:min-height={level > 0 ? "1rem" : undefined}
    style:height={level > 0 ? undefined : "100%"}
  ></div>
  <p class="label">{label}</p>
</div>

<style>
  .container {
    display: flex;
    align-items: center;
    width: 100%;
    height: 1.5rem;
  }

  .label {
    color: var(--gray-35);
  }

  .tick {
    width: 0.5rem;
    border-left: 1px solid var(--gray-35);
  }
</style>
