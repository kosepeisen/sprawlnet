#include <iostream>
#include <sstream>

#include "string_util.h"

namespace omninet {

std::string int_to_string(int number) {
    std::ostringstream out(std::ostringstream::out);
    out << number;
    return out.str();
}

}
