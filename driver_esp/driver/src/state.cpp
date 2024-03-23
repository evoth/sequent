#include "state.h"

template <class T>
void StateManager<T>::addState(int layer, T state) {
  int topIndex = getTopIndex();
  if (topIndex <= layer) {
    actOnDiff(topIndex == -1 ? defaultState : stateLayers[topIndex], state);
  }
  if (stateLayers.size() <= layer)
    stateLayers.resize(layer + 1);
  stateLayers[layer] = state;
  if (used.size() <= layer)
    used.resize(layer + 1);
  used[layer] = true;
}

template <class T>
void StateManager<T>::removeState(int layer) {
  int topIndex = getTopIndex();
  if (used.size() <= layer)
    used.resize(layer + 1);
  used[layer] = false;
  int newTopIndex = getTopIndex();
  if (newTopIndex < topIndex) {
    actOnDiff(stateLayers[topIndex],
              newTopIndex == -1 ? defaultState : stateLayers[newTopIndex]);
  }
}