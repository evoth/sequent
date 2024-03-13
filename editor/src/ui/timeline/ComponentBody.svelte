<script lang="ts">
  import { Component } from "../../data/sequence";

  export let component: Component;
  export let disabled = false;
  export let shadow = false;
  export let highlight = false;

  let child = component.child.getManageableChild();
</script>

<div
  class="container"
  class:disabled
  class:shadow
  class:highlight
  style:background-color={`hsl(${child.hue}deg var(--component-saturation) var(--component-lightness))`}
>
  <div class="spacer"></div>
  <div class="label">
    {child.name}
  </div>
  <div class="spacer"></div>
</div>

<style>
  .container {
    position: absolute;
    width: 100%;
    border-radius: 1rem;
    padding: 1rem 0.2rem;
    cursor: pointer;
    height: 90%;
    top: 5%;
    overflow: hidden;
    display: flex;
  }

  .disabled {
    cursor: not-allowed;
    opacity: 50%;
  }

  .shadow {
    box-shadow:
      0 20px 25px -5px var(--shadow),
      0 8px 10px -6px var(--shadow);
  }

  .highlight {
    border: 2px solid var(--gray-35);
  }
  .highlight::after {
    content: "";
    position: absolute;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
    border-radius: 1rem;
    background-color: var(--gray-35);
    opacity: 15%;
  }

  .spacer {
    width: 0.8rem;
  }
</style>
