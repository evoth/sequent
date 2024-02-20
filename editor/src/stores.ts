import { Sequence } from './data/sequence';
import { writable } from 'svelte/store';

export const selectedSequence = writable<Sequence | undefined>();
export const modalOpen = writable(false);