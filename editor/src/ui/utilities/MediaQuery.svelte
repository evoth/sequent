<script lang="ts">
  import { onMount } from "svelte";

  export let minWidth: number | undefined = undefined;
  export let maxWidth: number | undefined = undefined;
  let query = `${minWidth ? `(min-width: ${minWidth}px)` : ""}${
    minWidth && maxWidth ? " and " : ""
  }${maxWidth ? `(max-width: ${maxWidth - 1}px)` : ""}`;

  let mql: MediaQueryList | undefined = undefined;
  let mqlListener = (ev: MediaQueryListEvent) => (matches = ev.matches);
  let matches = false;

  onMount(() => {
    removeActiveListener();
    addNewListener();
    return () => {
      removeActiveListener();
    };
  });

  function addNewListener() {
    mql = window.matchMedia(query);
    mql.addEventListener("change", mqlListener);
    matches = mql.matches;
  }

  function removeActiveListener() {
    if (!mql) return;
    mql.removeEventListener("change", mqlListener);
  }
</script>

{#if matches}
  <slot />
{/if}
