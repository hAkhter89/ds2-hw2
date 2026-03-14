#ifndef __BASE_HASH_TABLE__
#define __BASE_HASH_TABLE__

#include <stdint.h>

#include <iostream>
#include <map>
#include <vector>

struct Entry {
    int    key;
    bool   occupied;
    size_t probeDistance;
    Entry() : key(0), occupied(false), probeDistance(0) {}
    Entry(int k, bool occ, size_t dist)
        : key(k), occupied(occ), probeDistance(dist) {}
};

class BaseHashTable {
public:

    BaseHashTable(size_t cap);
    virtual ~BaseHashTable() {}

    virtual bool insert(int key)                        = 0;
    virtual bool search(int key, int& probeCount) const = 0;
    virtual bool remove(int key)                        = 0;

    double getAvgProbeLength() const;
    int    getMaxProbeLength() const;

    friend std::ostream& operator<<(std::ostream& os, const BaseHashTable& ht) {
        for (size_t i = 0; i < ht.capacity; i++) {
            if (ht.table[i].occupied)
                os << "[" << i << "] key=" << ht.table[i].key
                   << " dist=" << ht.table[i].probeDistance << "\n";
            else
                os << "[" << i << "] ---\n";
        }
        return os;
    }

    std::map<int, int> getProbeDistribution() const;

protected:

    size_t             capacity;
    size_t             size;
    std::vector<Entry> table;

    int hash_fn(int key) const;
};

#endif
