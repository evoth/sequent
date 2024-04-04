## TODO
- Destructors?
- Add way to reset state
- Make it good
- Keep using smart pointers instead of raw pointer nonsense
  - Try to get rid of references as well where appropriate?
- Add disconnect functions for everything and make use of cameraConnected variables (e.g. stop keepalive if not isConnected)
  - Or have an isConnected member of Device class
- Concurrent sequences
- Ability to enable/disable live view for M200 (overheating prevention)