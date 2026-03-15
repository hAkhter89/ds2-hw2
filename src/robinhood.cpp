#include "../include/robinhood.hpp"

RobinHoodTable::RobinHoodTable(size_t size) : BaseHashTable(size) {}

bool RobinHoodTable::insert(int key) {
  if (size >= capacity) return false; // Table is full

  // temporary swaping entry
  Entry temp(key, true, 0);
  int index = hash_fn(key);

  while (table[index].occupied) {
    if (table[index].key == key) return false;

    if (temp.probeDistance > table[index].probeDistance) {
      Entry swapTemp = temp;
      temp = table[index];
      table[index] = swapTemp;
    }

    index = (index + 1) % capacity;
    temp.probeDistance++;
  }

  table[index] = temp;
  size++;

  return true;
}

// Early Termination
bool RobinHoodTable::search(int key, int &probeCount) const {
  int index = hash_fn(key);
  probeCount = 0;
  size_t distance = 0; // Tracks how far we've searched from the target's home
  size_t count = 0;

  while (table[index].occupied && count < capacity) {
    probeCount++;

    if (table[index].key == key) {return true;}

    // EARLY TERMINATION: if we've already traveled more than probe distance, the entry cannot be any further.
    if (table[index].probeDistance < distance) {return false;}

    index = (index + 1) % capacity;
    distance++;
    count++;
  }

  if (!table[index].occupied && count < capacity) {
    probeCount++;
  }

  return false;
}

bool RobinHoodTable::remove(int key) {
  int index = hash_fn(key);
  size_t distance = 0;
  size_t count = 0;

  while (table[index].occupied && count < capacity) {
    if (table[index].key == key) break;

    if (table[index].probeDistance < distance) return false; // Early termination

    index = (index + 1) % capacity;
    distance++;
    count++;
  }

  if (!table[index].occupied || count == capacity) {
    return false;
  }

  // delete key
  table[index].occupied = false;
  size--;

  // Backward shift
  int i = index;
  int j = (i + 1) % capacity;

  // pull elements back if next element is occupied AND (probeDistance > 0).
  while (table[j].occupied && table[j].probeDistance > 0) {
    table[i] = table[j];         // Shift element back
    table[i].probeDistance--;    // closer to home position

    table[j].occupied = false;
    i = j;                       // move one step forward and repeat
    j = (j + 1) % capacity;
  }
  

  return true;
}
