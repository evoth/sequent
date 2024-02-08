<script lang="ts">
  import { Manager } from "../data/manager";
  import { Repeat, RepeatProps, RepeatPropsError } from "../data/repeat";
  import { Timestamp } from "../data/timestamp";

  const timestampManager = new Manager<Timestamp>();
  const rootTimestamp = new Timestamp(timestampManager, 0);
  const testProps = new RepeatProps(
    {
      start: new Timestamp(timestampManager, 0, rootTimestamp),
      duration: 60,
      interval: 5,
    },
    ["start", "duration", "interval"],
    false,
    false
  );
  let actionDuration = 2;
  let [error, solved] = testProps.validate(actionDuration);

  let duration: number, interval: number, repetitions: number;
  const scale = 10; // px/s
  if (error === RepeatPropsError.None) {
    console.log(solved);
    duration = solved!.duration!;
    interval =
      solved!.interval! + (testProps.includeChildDuration ? 0 : actionDuration);
    repetitions = solved!.repetitions!;
  }
</script>

<div class="container">
  {#if error === RepeatPropsError.None}
    <div class="repeat" style:width="{duration * scale}px">
      {#each Array(repetitions) as _, i}
        <div
          class="action"
          style:width="{actionDuration * scale}px"
          style:left="{i * interval * scale}px"
        ></div>
      {/each}
    </div>
  {:else}
    <p>{error}</p>
  {/if}
</div>

<style>
  .container {
    width: 100%;
    height: 100%;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
  }

  .repeat {
    height: 50px;
    background-color: blueviolet;
    position: relative;
  }

  .action {
    height: 80%;
    top: 10%;
    background-color: aqua;
    position: absolute;
    border: 2px solid blue;
  }
</style>
