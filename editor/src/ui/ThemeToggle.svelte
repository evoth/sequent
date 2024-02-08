<script lang="ts">
  export let theme =
    localStorage.getItem("theme") ||
    (window.matchMedia("(prefers-color-scheme: dark)").matches
      ? "dark"
      : "light");

  const update = () => {
    const classList = window.document.documentElement.classList;
    if (theme == "dark") {
      classList.add("dark");
    } else {
      classList.remove("dark");
    }
  };

  const toggle = () => {
    theme = theme == "light" ? "dark" : "light";
    localStorage.setItem("theme", theme);
    update();
  };

  update();
</script>

<button on:click={toggle} title="Switch theme" class="inverse">
  {#if theme == "light"}
    <svg
      xmlns="http://www.w3.org/2000/svg"
      width="32"
      height="32"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      stroke-width="2"
      stroke-linecap="round"
      stroke-linejoin="round"
      ><circle cx="12" cy="12" r="5" /><path
        d="M12 1v2M12 21v2M4.2 4.2l1.4 1.4M18.4 18.4l1.4 1.4M1 12h2M21 12h2M4.2 19.8l1.4-1.4M18.4 5.6l1.4-1.4"
      /></svg
    >
  {:else}
    <svg
      xmlns="http://www.w3.org/2000/svg"
      width="32"
      height="32"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      stroke-width="2"
      stroke-linecap="round"
      stroke-linejoin="round"
      ><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path></svg
    >
  {/if}
</button>

<style>
  button {
    padding: 8px 8px;
    min-width: 48px;
    display: flex;
    align-items: center;
  }
</style>
