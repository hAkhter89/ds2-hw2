#include "../include/linear.hpp"

LinearProbingTable::LinearProbingTable(size_t size) : BaseHashTable(size) {}

bool LinearProbingTable::insert(int key) {
  if (size >= capacity) {
  return false;
  }
  int index =  hash_fn(key);
  int probeCount = 0;
  while (table[index].occupied) {
    if (table[index].key == key) {
      return false; // Key already exists
      }
    index = (index + 1) % capacity; // Linear Probe
    probeCount++;
  }
  table[index].key = key;
  table[index].occupied = true;
  table[index].probeDistance = probeCount;
  size++;
  return true;
}

bool LinearProbingTable::search(int key, int &probeCount) const {
  int index = hash_fn(key);
  probeCount = 0;
  size_t count = 0;

  while(table[index].occupied && count < capacity) {
    probeCount++;
    if (table[index].key == key) {
      return true; // found
    }
    index = (index + 1) % capacity;
    count++;
  }
  if (!table[index].occupied && count < capacity) {
    probeCount++;
  }

  return false; // not found
}

bool LinearProbingTable::remove(int key) {
  int index = hash_fn(key);
  size_t count = 0;

  // finding the key
  while (table[index].occupied && count < capacity) {
    if (table[index].key == key) {
      break;
    }
    index = (index + 1) % capacity;
    count++;
  }

  // empty slot/ or count == capacity means key not found
  if (!table[index].occupied || count == capacity) {
    return false;
  }

  // delete key
  table[index].occupied = false;
  size--;

  // BACKWARD SHIFT
  int i = index;
  int j = (i + 1) % capacity;

  while (table[j].occupied) {
    int h_k = hash_fn(table[j].key);
    bool shift = false;

    // shift an element at j back to i IF its original hash position
    // is not cyclically between i and j.
    if (i < j) {
      if (h_k <= i || h_k > j) shift = true;
    } else {
      if (h_k <= i && h_k > j) shift = true;
    }

    if (shift) {
      table[i] = table[j];

      // decrease probe distance since it moved closer to home
      int difference = (j >= i) ? (j - i) : (capacity - i + j);
      table[i].probeDistance -= difference;

      table[j].occupied = false;
      i = j;
      }
      j = (j + 1) % capacity;
    }

  return true;
}
