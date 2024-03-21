# Sequent Editor

This will be a web-based editor for composing sequences using actions and parameters that are specified depending on the intended target system (backend).

## Plan: FOCUS ON MVP!!

Not going to try to make anything perfect, just need to get it functional before I worry about any non-essential details

## TODO

- The program should never save an invalid state. Make validation stricter and use default values to mitigate empty ones.
  - Repeat should always have valid props and child... should always store a "solved" version which can be reverted back to
    - RepeatProps props could be readonly, and Repeat props could use getters/setters to enforce stuff
  - So unless we want a bad user experience, the program will have to deal with invalid states, mainly arising from invalid Repeat configurations (deep nested Repeat child going from finite to infinite duration, for instance)
- Give errors IDs for easier error tracking?
- Add methods to add/remove stuff (layers to sequences, repeats to layers, etc.)
- Just learned that parameters properties exist, so I should probably convert constructors to use that as much as possible
- Better keyboard controls
  - Modals (pressing enter should submit, escape should cancel)
  - Sequence tabs
  - Navigation between panes?
  - Dropdown compliance
- ARIA attributes on things like dropdowns
- Use correct HTML elements instead of just divs within divs within divs within...
- Fix weird white flash in sequence tabs area on refresh
- Investigate using margins instead of absolute positioning in cases where it's feasible
- Figure out mess of binding and be intentional with indices in each blocks
  - Get rid of updateIndex?
- Weird cases to handle later
  - Duration changes (automatically move component to new layer if overlap?)
    - Sequence can change length whenever one of its components are modified, moved, removed, added, etc.
    - Duration can go to zero when add components removed from sequence
      - Then that sequence will be a "ghost" that reappears when the sequence has a positive duration again (not desired behavior, to be clear)
  - Sequence tabs are in reverse order of dependency due to serialization stuff--maybe we'll leave it like this until we support tab reordering? But it would make sense to have them reversed of how they are right now
- Disable infinity in negative direction for now? (still code defensively for it but don't specifically build features to support it, and don't allow it to be selected by user)
  - Actually, we don't need infinity in *either* direction for MVP, so maybe let's not allow it for now?
- Simplify draggable class to only have separate element when actually dragging, and only update info as needed (right now we keep it so that it can track mousemove events and stuff)
- Simplify string indices from weird formatted string -> array
- Add info hints (little circles with i or ? to explain things)
- **FIX FIREFOX (BUGGY DB BEHAVIOR; MAKE IT BETTER)**
- Adjust component saturation/lightness (brighter? better contrast on light mode)
- Add date display settings (MM/DD vs DD/MM for example)
- Manual timezone support (right now everything is local timezone)
- Show child duration of component in repetitions section
- Show parameter/repetition info on component body
- Show representation of child/repetitions on component body
- Visual separation between groups of related inputs in repetition section