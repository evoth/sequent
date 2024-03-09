<script lang="ts">
  import { Component, Layer, Sequence } from "../../data/sequence";
  import Draggable from "./Draggable.svelte";
  import LayerComponentWrapper from "./LayerComponentWrapper.svelte";

  export let offset: number;
  export let duration: number;
  export let layer: Layer;
  export let index: number;
  export let tileIndex: number;
  export let tilesEnd: number;
  export let sequence: Sequence;

  $: components = findComponents(tilesEnd, tileIndex);

  function findComponents(
    newTilesEnd: number,
    newTileIndex: number
  ): [component: Component, start: number, end: number][] {
    const newComponents: [Component, number, number][] = [];
    // TODO: Make stuff work with absolute sequences (will have to change a lot of other stuff too)
    // TODO: use childBounds from Layer.validate()
    for (const child of layer.children) {
      const validation = child.validate();

      if (validation.solved === undefined) continue;

      let childStart: number;
      if (validation.solved.start === undefined) {
        if (newTileIndex == 0) {
          childStart = offset;
        } else {
          continue;
        }
      } else {
        childStart = validation.solved.start.getOffset()[0];
      }

      let childEnd: number;
      if (validation.solved.end === undefined) {
        childEnd = newTilesEnd;
      } else {
        childEnd = Math.min(newTilesEnd, validation.solved.end.getOffset()[0]);
      }

      if (
        (childStart >= offset || (childEnd > offset && newTileIndex === 0)) &&
        childStart < offset + duration
      ) {
        newComponents.push([child, Math.max(offset, childStart), childEnd]);
      }
    }
    return newComponents;
  }

  function removeComponent(component: Component) {
    layer.children.delete(component);
  }
</script>

<div
  class="container"
  style:transform={`translateY(${-index * 100}%)`}
  style:background-color={`var(--gray-${index % 2 == 0 ? "93" : "95"})`}
>
  {#each components as [component, start, end] (component)}
    <Draggable
      getComponent={() => component}
      hideChildOnDrag
      bind:sequence
      removeComponent={() => removeComponent(component)}
    >
      <LayerComponentWrapper
        {offset}
        tileDuration={duration}
        {component}
        {start}
        {end}
      />
    </Draggable>
  {/each}
</div>

<style>
  .container {
    position: absolute;
    height: 100%;
    width: 100%;
    bottom: 0;
  }
</style>
