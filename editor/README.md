# Sequent Editor

This will be a web-based editor for composing sequences using actions and parameters that are specified depending on the intended target system (backend).

## TODO

- The program should never save an invalid state. Make validation stricter and use default values to mitigate empty ones.
  - Need to check all logic, but especially anything labeled "validate"... also, right now Parameter.validate will accept empty value because T can be "any", which includes null types
  - So unless we want a bad user experience, the program will have to deal with invalid states, mainly arising from invalid Repeat configurations (deep nested Repeat child going from finite to infinite duration, for instance)
- Change methods to getters/setters as appropriate
  - Setters should throw an error if validation fails
