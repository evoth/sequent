<script lang="ts">
  import Portal from "svelte-portal";

  export let hue: number;

  let trackWidth = 0;
  let thumbWidth = 0;
  let isDragging = false;
  let trackElement: HTMLElement;

  function dragStart(event: MouseEvent) {
    isDragging = true;
    dragMove(event);
  }

  function dragMove(event: MouseEvent) {
    if (!isDragging) return;
    const startX = trackElement.getBoundingClientRect().left;
    hue = Math.floor(
      Math.max(
        0,
        Math.min(
          360,
          ((event.clientX - startX - thumbWidth / 2) /
            (trackWidth - thumbWidth)) *
            360
        )
      )
    );
  }
</script>

<div class="container">
  <button
    class="track"
    bind:this={trackElement}
    bind:clientWidth={trackWidth}
    on:mousedown={dragStart}
  >
    <div
      class="thumb"
      bind:clientWidth={thumbWidth}
      style:background-color={`hsl(${hue}deg var(--component-saturation) var(--component-lightness))`}
      style:transform={`translateX(calc((${trackWidth}px - 100%) * (${hue} / 360)))`}
    ></div>
  </button>
</div>

{#if isDragging}
  <Portal target="body">
    <!-- svelte-ignore a11y-no-static-element-interactions -->
    <div
      class="dragArea"
      on:mousemove={dragMove}
      on:mouseup={() => (isDragging = false)}
    ></div></Portal
  >
{/if}

<style>
  .container {
    height: 2.1rem;
  }

  .track {
    height: 1.5rem;
    background: linear-gradient(
      to right in hsl increasing hue,
      hsl(0deg var(--component-saturation) var(--component-lightness)),
      hsl(359deg var(--component-saturation) var(--component-lightness))
    );
    border-radius: 0.5rem;
    position: relative;
    padding: 0;
    width: 100%;
  }

  .thumb {
    position: absolute;
    height: 2.1rem;
    top: -0.4rem;
    width: 1.5rem;
    border-radius: 0.5rem;
    border: solid 2px var(--gray-50);
    transition: border-color 0.2s;
  }

  .track:hover > .thumb {
    border-color: var(--gray-35);
  }
  .track:focus > .thumb {
    border-color: var(--gray-15);
  }

  .dragArea {
    position: fixed;
    top: 0;
    bottom: 0;
    right: 0;
    left: 0;
  }
</style>
