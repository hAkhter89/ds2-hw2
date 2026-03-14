#ifndef __LINEAR_PROBING_TABLE__
#define __LINEAR_PROBING_TABLE__

#include "basehashtable.hpp"

class LinearProbingTable : public BaseHashTable {
public:

    LinearProbingTable(size_t size);

    bool insert(int key) override;
    bool search(int key, int &probeCount) const override;
    bool remove(int key) override;

    bool search(int key) const {
        int dummy;
        return search(key, dummy);
    }
};

#endif