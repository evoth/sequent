# Sequent Editor

This will be a web-based editor for composing sequences using actions and parameters that are specified depending on the intended target system (backend).

## TODO

- The program should never save an invalid state. Make validation stricter and use default values to mitigate empty ones.
  - Repeat should always have valid props and child... should always store a "solved" version which can be reverted back to
    - RepeatProps props could be readonly, and Repeat props could use getters/setters to enforce stuff
  - So unless we want a bad user experience, the program will have to deal with invalid states, mainly arising from invalid Repeat configurations (deep nested Repeat child going from finite to infinite duration, for instance)
- Change methods to getters/setters as appropriate
  - Setters should throw an error if validation fails
- Give errors IDs for easier error tracking?
- Add methods to add/remove stuff (layers to sequences, repeats to layers, etc.)
- **Deep sequence dependency validation (no cycles allowed)**
  - Need to do topological sort for managers before serialization, so we can use that to check for cycles as well?
- Just learned that parameters properties exist, so I should probably convert constructors to use that as much as possible
- Better keyboard controls
  - Modals (pressing enter should submit, escape should cancel)
  - Sequence tabs
  - Navigation between panes?

## Plan: FOCUS ON MVP!!

Not going to try to make anything perfect, just need to get it functional before I worry about any non-essential details