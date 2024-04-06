<script>
  export let title;
  export let logs;
</script>

<div>
  <strong>{title}:</strong>
  {#each logs as log}
    <p class={log.isError ? "error" : "success"}>
      {log.message}
    </p>
  {/each}
</div>
