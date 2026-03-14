#ifndef __ROBIN_HOOD_PROBING_TABLE_HPP__
#define __ROBIN_HOOD_PROBING_TABLE_HPP__

#include "basehashtable.hpp"

class RobinHoodTable : public BaseHashTable {
public:

    RobinHoodTable(size_t size);

    bool insert(int key) override;
    bool search(int key, int &probeCount) const override;
    bool remove(int key) override;

    bool search(int key) const {
        int dummy;
        return search(key, dummy);
    }
};

#endif