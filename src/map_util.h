#ifndef MAP_UTIL_H
#define MAP_UTIL_H

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

using std::map;
using std::vector;

namespace omninet {

template <typename K, typename V>
void delete_all_map_values(map<K, V> the_map) {
    typename map<K, V>::const_iterator it;
    for (it = the_map.begin(); it != the_map.end(); ++it) {
        delete it->second;
    }
}

template <typename T>
void delete_all_vector_values(vector<T> the_vector) {
    typename vector<T>::const_iterator it;
    for (it = the_vector.begin(); it != the_vector.end(); ++it) {
        delete *it;
    }
}

}

#endif
