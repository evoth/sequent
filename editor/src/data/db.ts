import type { Writable } from "svelte/store";

const STORE_NAME = "persistence";

// TODO: implement error handling
export class DB {
  readonly dbName: string;
  db: Promise<IDBDatabase>;

  constructor(dbName: string) {
    this.dbName = dbName;

    const openRequest = indexedDB.open(this.dbName);

    this.db = new Promise<IDBDatabase>((resolve, reject) => {
      openRequest.onupgradeneeded = () => {
        const newDb = openRequest.result;
        newDb.createObjectStore(STORE_NAME);
        resolve(newDb);
      };

      openRequest.onsuccess = () => {
        resolve(openRequest.result);
      };
    });
  }

  // TODO: generalize to other types besides Project?
  async initAutosave<T>(
    autosave: Writable<T>,
    autosaveName: string,
    serialize: (value: T) => any = (value) => value,
    deserialize: (value: any) => T = (value) => value
  ) {
    // Load data
    const json = await this.get(autosaveName);
    if (json !== undefined) {
      autosave.set(deserialize(json));
    }

    // Autosave after 1 second of no activity
    let ticker = 0;
    autosave.subscribe((value) => {
      ticker++;
      const oldTicker = ticker;

      setTimeout(() => {
        if (ticker !== oldTicker) return;
        console.log("hey");
        // Store serialized object in IndexedDB
        this.set(autosaveName, serialize(value));
      }, 5000);
    });
  }

  async set(key: string, value: any) {
    const objectStore = (await this.db)
      .transaction(STORE_NAME, "readwrite")
      .objectStore(STORE_NAME);

    objectStore.put(value, key);
  }

  async get(key: string): Promise<any> {
    const objectStore = (await this.db)
      .transaction(STORE_NAME, "readonly")
      .objectStore(STORE_NAME);

    const getRequest = objectStore.get(key);
    return new Promise((resolve, reject) => {
      getRequest.onsuccess = () => {
        resolve(getRequest.result);
      };
    });
  }
}
