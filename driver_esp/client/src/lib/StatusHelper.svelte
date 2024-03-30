<script>
    export let title;
    export let logs;
</script>

<div>
    <strong>{title}:</strong>
    {#each logs as log}
        <p
        class={log.statusCode == 200 || log.statusCode == 0 ? "success" : "error"}
    >
        ({log.statusCode}) {log.message}
    </p>
    {/each}
</div>