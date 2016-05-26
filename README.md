## Example

Given a logger component, which is required to have both pattern and severity parameters to be configured and an optional timestamp parameter. Let's write a compile-time checked builder for it.

```c++
#pragma once

#include <string>

#include <mxbuilder/builder.hpp>

struct pattern_component {
    using tag = required_tag;
    using type = std::string;

    template<class Parent>
    struct setter {
        auto pattern(type v) {
            pattern_component::validate(v);
            return static_cast<Parent*>(this)->template set<pattern_component>(std::move(v));
        }
    };

private:
    /// \note just an example how to hide builder arguments validation.
    static auto validate(const type& v) -> void;
};

struct severity_component {
    using tag = required_tag;
    using type = int;

    template<class Parent>
    struct setter {
        auto severity(type v) {
            return static_cast<Parent*>(this)->template set<severity_component>(std::move(v));
        }
    };
};

struct timestamp_component {
    using tag = optional_tag;
    using type = long long;

    template<class Parent>
    struct setter {
        auto timestamp(type v) {
            return static_cast<Parent*>(this)->template set<timestamp_component>(std::move(v));
        }
    };
};

struct logger_t {
    virtual ~logger_t() = default;
    virtual auto name() -> std::string = 0;
};

struct builder_t :
    public builder<
        std::unique_ptr<logger_t>,
        pattern_component,
        severity_component,
        timestamp_component
    >
{
private:
    auto complete(std::string pattern, int severity, std::optional<long long> timestamp) ->
        std::unique_ptr<logger_t> override;
};

int main() {
    // Won't compile, because severity component is required.
    //auto logger = builder_t()
    //    .pattern("{}")
    //    .build();

    // Compiles perfectly even without optional timestamp component.
    auto logger = builder_t()
        .pattern("{}")
        .severity(42)
        .build();

    std::cout << logger->name() << std::endl;

    return 0;
}
```

## How it works

Builder is considered as a state machine which has `build()` method enabled if there are no required components left unconfigured. Each `set` method invocation (which is hidden by CRTP setters for each component) moves the state machine one step forward.

Internally each argument is saved in a tuple of optional of component type, which is unpacked at the build time.

## What's left to do

- [ ] Saving arguments by index, not type.
- [ ] Repeated parameters (both for required and optional).

## Requirements

C++1z, but theoretically can be backported to pure C++11.
