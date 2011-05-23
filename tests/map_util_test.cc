#include <map>
#include "gtest/gtest.h"

#include "map_util.h"

using std::map;

bool delete_called = false;

class A {
    public:
    ~A() {
        delete_called = true;
    }
};

namespace omninet {

TEST(map_util, erase_no_deletion) {
    delete_called = false;

    map<int, A*> *my_map = new map<int, A*>;
    A *a = new A;
    (*my_map)[0] = a;
    delete my_map;
    EXPECT_FALSE(delete_called);
    if (!delete_called) {
        delete a;
    }
}

TEST(map_util, delete_all_map_values) {
    map<int, A*> my_map;
    A *a = new A;
    my_map[0] = a;
    delete_all_map_values(my_map);
    EXPECT_TRUE(delete_called);
}

}
