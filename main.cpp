#include <iostream>

#include "example.hpp"

int main() {
    auto logger = builder_t()
        .pattern("{}")
        .severity(42)
        .build();

    std::cout << logger->name() << std::endl;

    return 0;
}
