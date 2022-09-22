#ifndef DLPLAN_SRC_UTILS_HASH_UTILS_H_
#define DLPLAN_SRC_UTILS_HASH_UTILS_H_

#include <vector>
#include <array>

#include "MurmurHash3.h"


namespace dlplan::utils::hashing {

/**
 * For combining hash value we use the boost::hash_combine one-liner.
 * https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
 */
template <class T>
void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

}

#endif
