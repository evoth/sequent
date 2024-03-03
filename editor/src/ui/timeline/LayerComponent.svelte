<script lang="ts">
  import { Component } from "../../data/sequence";

  export let offset: number;
  export let tileDuration: number;
  export let component: Component;
  export let start: number;
  export let end: number;

  let child = component.child.getManageableChild();
</script>

<div
  class="container"
  style:background-color={`hsl(${child.hue}deg var(--component-saturation) var(--component-lightness))`}
  style:left={`${((start - offset) / tileDuration) * 100}%`}
  style:width={`${((end - start) / tileDuration) * 100}%`}
>
  <p>{child.name}</p>
</div>

<style>
  .container {
    position: absolute;
    height: 90%;
    top: 5%;
    border-radius: 1rem;
    padding: 1rem;
  }
</style>
