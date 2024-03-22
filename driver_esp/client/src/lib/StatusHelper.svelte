<script>
    export let name;
    export let logs;
</script>

<div>
    <strong>{name} logs:</strong>
    {#each logs as log}
        <p
        class:success={log.statusCode == 200}
        class:warning={log.statusCode == 0}
        class:error={log.statusCode != 200 && log.statusCode != 0}
    >
        ({log.statusCode}) {log.message}
    </p>
    {/each}
</div>