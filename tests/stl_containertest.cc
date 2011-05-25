#include <vector>
#include <tr1/memory>

#include "gtest/gtest.h"

using std::vector;
using std::tr1::shared_ptr;

bool delete_called = false;

class A {
    public:
    ~A() {
        delete_called = true;
    }
};

namespace sprawlnet {

TEST(std_map, erase_no_deletion) {
    delete_called = false;

    vector<A*> *my_vector = new vector<A*>;
    A *a = new A;
    my_vector->push_back(a);
    delete my_vector;
    EXPECT_FALSE(delete_called);
    if (!delete_called) {
        delete a;
    }
}

TEST(std_vector, shader_ptr_container) {
    delete_called = false;

    shared_ptr<A> *a = new shared_ptr<A>(new A);
    vector<shared_ptr<A> > *my_vector = new vector<shared_ptr<A> >;
    my_vector->push_back(*a);
    delete a;

    EXPECT_FALSE(delete_called);
    delete my_vector;
    EXPECT_TRUE(delete_called);
}

}
