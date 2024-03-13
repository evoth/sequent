<script lang="ts">
  import { Svroller } from "svrollbar";

  export let fullWidth = false;
</script>

<div class="container" style:width={fullWidth ? "100%" : undefined}>
  <Svroller width="100%" height="100%">
    <slot />
  </Svroller>
</div>

<style>
  .container {
    background-color: var(--gray-90);
    width: 360px;
    height: 100%;
    border-bottom: var(--border-style);
  }
</style>
