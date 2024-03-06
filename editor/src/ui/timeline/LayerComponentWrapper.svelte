<script lang="ts">
  import { Component } from "../../data/sequence";
  import ComponentBody from "./ComponentBody.svelte";

  export let offset: number;
  export let tileDuration: number;
  export let component: Component;
  export let start: number;
  export let end: number;

  let child = component.child.getManageableChild();
</script>

<div
  class="container"
  style:left={`${((start - offset) / tileDuration) * 100}%`}
  style:width={`${((end - start) / tileDuration) * 100}%`}
>
  <ComponentBody {component} />
</div>

<style>
  .container {
    position: absolute;
    height: 100%;
  }
</style>
