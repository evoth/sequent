<script lang="ts">
  export let title: string;
</script>

<div class="container">
  <div class="title">
    <h2>{title}</h2>
  </div>
  <div class="sections">
    <slot />
  </div>
</div>

<style>
  .container {
    background-color: var(--gray-90);
    width: 360px;
    height: 100%;
  }

  .title {
    background-color: var(--gray-90);
    padding: 0.8rem;
  }

  .title > h2 {
    font-size: 1.3rem;
  }
</style>
