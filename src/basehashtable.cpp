#include "../include/basehashtable.hpp"

BaseHashTable::BaseHashTable(uint64_t cap) : capacity(cap), size(0) {
  table.resize(capacity);
}

int BaseHashTable::hash_fn(int key) const {
  return key % capacity;
}

double BaseHashTable::getAvgProbeLength() const {
  if (size == 0) {
    return 0.0;
  }
  double totalProbeLength = 0.0;
  for (int i = 0; i < capacity; i++) {
    if (table[i].occupied) {
       totalProbeLength += table[i].probeDistance;
    }
  }
  return totalProbeLength / size;
}

int BaseHashTable::getMaxProbeLength() const {
  if (size == 0) {
    return 0;
  }
  int maxProbeLength = 0;
  for (int i = 0; i < capacity; i++) {
    if (table[i].occupied) {
       if (table[i].probeDistance > maxProbeLength) {
       maxProbeLength = table[i].probeDistance;
       }
    }
  }
  return maxProbeLength;
}

std::map<int, int> BaseHashTable::getProbeDistribution() const {
  std::map<int, int> distribution;
  for (int i = 0; i < capacity; i++) {
    if (table[i].occupied) {
      distribution[table[i].probeDistance]++; // i.e if probe distance = 7 distrubtion[7th entry]+1
    }
  }
  return distribution;

}
