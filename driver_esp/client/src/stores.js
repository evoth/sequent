import { readable, writable } from "svelte/store";

export const isConnected = writable(false);
export const espIP = writable("192.168.4.1");
export const state = writable({
  statusCode: 0,
  statusMsg: "Page loaded.",
  cameraConnected: false,
  isRunning: false,
  cameraIP: "192.168.4.7",
  actionIndex: 0,
  timeUntilNext: 0,
  totalActions: 0,
});
export const isLoading = writable(false);
export const socket = writable();
