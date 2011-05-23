#ifndef MAP_UTIL_H
#define MAP_UTIL_H

#include <map>

using std::map;

namespace omninet {

template <class K, class V>
void delete_all_map_values(map<K, V> the_map) {
    map<K, V>::const_iterator it;
    for (it = the_map.begin(); it != the_map.end(); ++it) {
        delete it->second;
    }
}

}

#endif
