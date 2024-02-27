<script lang="ts">
  import { Project } from "../data/project";
  import { toJSONString } from "../data/serialization";
  import { project } from "../stores";
  import Dropdown from "./Dropdown.svelte";
  import Modal from "./Modal.svelte";

  let modalOpen = false;
  let showOptions: boolean;
  let fileHandle: FileSystemFileHandle | undefined = undefined;
  let fileInput: HTMLInputElement | undefined = undefined;

  let modalTitle = "";
  let modalDescription = "";

  async function open() {
    showOptions = false;
    modalTitle = "Open location...";
    modalDescription = "Choose project file to open using the popup window.";
    modalOpen = true;
    if ("showSaveFilePicker" in window) {
      const opts = {
        id: "project",
        types: [
          {
            description: "Sequent project file",
            accept: { "application/json": [".sqp"] },
          },
        ],
      };
      try {
        // TODO: try persisting across sessions using IndexedDB (see https://stackoverflow.com/a/65326027)
        [fileHandle] = await (window as any).showOpenFilePicker(opts);
        if (fileHandle === undefined) return;
        const file = await fileHandle.getFile();
        const text = await file.text();
        $project = Project.fromJSON(JSON.parse(text));
      } catch {}
      modalOpen = false;
    } else {
      if (fileInput === undefined) return;
      fileInput.click();
    }
  }

  // Used to load file data from file input
  async function openOnChangeHelper() {
    modalOpen = false;
    if (fileInput === undefined || fileInput.files === null) return;
    if (fileInput.files?.length === 0) return;
    $project = Project.fromJSON(JSON.parse(await fileInput.files[0].text()));
  }

  // Used to load file data from file input
  function openOnCancelHelper() {
    modalOpen = false;
  }

  // TODO: keyboard shortcuts
  async function save() {
    if (fileHandle === undefined) throw Error("File handle is undefined.");
    const writable = await fileHandle.createWritable();
    await writable.write(toJSONString($project));
    await writable.close();
    showOptions = false;
    // TODO: show loading wheel and checkmark to indicate progress/completion?
  }

  async function saveAs() {
    showOptions = false;
    modalTitle = "Select file location...";
    modalDescription =
      "Choose where to save the project file using the popup window.";
    modalOpen = true;
    const opts = {
      id: "project",
      suggestedName: `${$project.name}.sqp`,
      types: [
        {
          description: "Sequent project file",
          accept: { "application/json": [".sqp"] },
        },
      ],
    };
    try {
      // TODO: try persisting across sessions using IndexedDB (see https://stackoverflow.com/a/65326027)
      fileHandle = await (window as any).showSaveFilePicker(opts);
    } catch {}
    await save();
    modalOpen = false;
  }

  // Used as fallback for browsers like Firefox that don't support File System Access API
  function download() {
    var a = document.createElement("a");
    a.href = URL.createObjectURL(
      new Blob([toJSONString($project)], { type: "application/json" })
    );
    a.download = `${$project.name}.sqp`;
    a.click();
    showOptions = false;
  }
</script>

<Dropdown closeCondition={modalOpen} bind:showOptions>
  <button
    class="menu-button"
    slot="button"
    let:openDropdown
    on:click={openDropdown}
    title="File options">File</button
  >
  <svelte:fragment slot="buttons">
    <button on:click={open}>Open</button>
    {#if "showSaveFilePicker" in window}
      {#if fileHandle !== undefined}
        <button on:click={save}>Save</button>
      {/if}
      <button on:click={saveAs}>Save as...</button>
    {:else}
      <button on:click={download}>Download</button>
    {/if}
  </svelte:fragment>
  <input
    on:change={openOnChangeHelper}
    on:cancel={openOnCancelHelper}
    bind:this={fileInput}
    type="file"
    accept=".sqp,application/json"
    style="position: absolute; visibility: hidden"
  />
  <Modal bind:isOpen={modalOpen} title={modalTitle} showActions={false}>
    <p>{modalDescription}</p>
  </Modal>
</Dropdown>
