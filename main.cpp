#include <iostream>

#include "example.hpp"

int main() {
    // Won't compile, because both required components weren't initialized.
    // auto logger = logger_builder_t()
    //     .build();

    auto logger = logger_builder_t()
        .pattern("{}")
        .severity(42)
        .build();

    std::cout << logger->name() << std::endl;

    return 0;
}
