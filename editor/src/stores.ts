import { writable } from "svelte/store";
import { ActionSet } from "./data/actionSet";
import { Project } from "./data/project";

// Delete test data and figure out what happens for new user
const actionSet = new ActionSet("Test");
// const param0 = new NumberParameter<number>(
//   actionSet.parameterManager,
//   "Test param",
//   "",
//   7,
//   undefined,
//   undefined,
//   undefined,
//   undefined,
//   "3"
// );
// const param1 = new StringParameter<string>(
//   actionSet.parameterManager,
//   "Test param",
//   "",
//   "defgecd",
//   undefined,
//   undefined,
//   "2"
// );
// const param2 = new NestedParameter<string>(
//   actionSet.parameterManager,
//   "Test param",
//   "",
//   "beanz",
//   new Map(Object.entries({ beanz: [param0], tim: [param1, param0], ".": [] })),
//   "1"
// );
// const param3 = new NestedParameter<string>(
//   actionSet.parameterManager,
//   "Test param",
//   "",
//   "beanz",
//   new Map(Object.entries({ beanz: [param2], tim: [param0], ".": [] })),
//   "0"
// );
// new Action(
//   actionSet.actionManager,
//   "Photo",
//   "Trigger shutter with given settings",
//   { defaultDuration: 1, durationParam: param0 },
//   [param0, param2, param1, param3]
// );
export const project = writable(new Project(actionSet));

// Open database
// TODO: clean up and implement error handling
// TODO: move to a separate file
const dbName = "sequent";
const storeName = "autosave";
const openRequest = indexedDB.open(dbName);

openRequest.onupgradeneeded = () => {
  const db = openRequest.result;
  db.createObjectStore(storeName);
};

openRequest.onsuccess = () => {
  const db = openRequest.result;

  // Load data
  const objectStore = db
    .transaction(storeName, "readonly")
    .objectStore(storeName);
  const getRequest = objectStore.get(storeName);
  getRequest.onsuccess = () => {
    if (getRequest.result === undefined) return;
    project.set(Project.fromJSON(getRequest.result));
  };

  // Autosave after 1 second of no activity
  let ticker = 0;
  project.subscribe((projectValue) => {
    ticker++;
    const oldTicker = ticker;

    setTimeout(() => {
      if (ticker !== oldTicker) return;
      // Store serialized project in IndexedDB
      const objectStore = db
        .transaction(storeName, "readwrite")
        .objectStore(storeName);
      const json = JSON.parse(
        JSON.stringify(projectValue, (key, value) =>
          value instanceof Map ? Object.fromEntries(value) : value
        )
      );
      objectStore.put(json, storeName);
    }, 1000);
  });
};
