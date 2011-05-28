/**
 * Copyright 2011 Kosepeisen.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */
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
