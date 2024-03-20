<script lang="ts">
  import { Svroller } from "svrollbar";
  import PanesLayout from "./ui/panes/PanesLayout.svelte";
  import TitleBar from "./ui/titleBar/TitleBar.svelte";
  import { BREAKPOINT_LG } from "./ui/utilities/breakpoints";

  let width = 0;
</script>

<main bind:clientWidth={width}>
  <TitleBar />
  {#if width < BREAKPOINT_LG}
    <div class="scrollContainer">
      <Svroller width="100%" height="100%">
        <PanesLayout {width} />
      </Svroller>
    </div>
  {:else}
    <PanesLayout {width} />
  {/if}
</main>

<style>
  main {
    height: 100%;
    display: flex;
    flex-direction: column;
  }

  .scrollContainer {
    flex: 1;
    min-height: 0;
  }
</style>
