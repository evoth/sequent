import { writable } from "svelte/store";

export const isConnected = writable(false);
export const espIP = writable("192.168.4.1");
// TODO: separate status of different components
export const state = writable({
  states: [],
  sequenceKey: {},
  cameras: [],
  cameraIP: "192.168.4.7",
});
export const isLoading = writable(false);
export const socket = writable();
