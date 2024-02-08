# Sequent

A flexible tool for composing and programmatically executing complex sequences of actions, including time-lapses, sensor readings, and more!

## Plan

The will be a web-based sequence editor to design a sequence for a given system (backend). Then, each system will have a "backend" program that parses the output of the web editor and executes the specified actions.

For each backend, there will be predefined "actions", the behavior of which can be modified based on given "parameters". The backend will have functions that translate the (probably JSON) serialized output of the web editor into the desired actions. The backend will also parse the timing and layering information present in the sequence data.

## Timing

There will be a "main" or "root" sequence, which contains all actions/sequences to be run and relates the timing of each to an absolute timestamp (either populated based on start time of the program or just a specific time). Each sequence can contain other sequences and actions. In both cases, there will be options to position, repeat, and layer them as needed. In this way, complex sequences can be constructed in a recursive manner.
