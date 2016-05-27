## Example

Given a logger component, which is required to have both pattern and severity parameters to be configured and an optional timestamp parameter. Let's write a compile-time checked builder for it.

```c++
#pragma once

#include <string>

#include <mxbuilder/builder.hpp>

struct logger_t {
    virtual ~logger_t() = default;
    virtual auto name() -> std::string = 0;
};

struct logger_builder_t;

template<>
struct builder_traits<logger_builder_t> {
    struct pattern_t {
        using tag = required_tag<1>;
        using type = std::string;

        template<class Parent>
        struct setter {
            auto pattern(type v) {
                pattern_t::validate(v);
                return static_cast<Parent*>(this)->template set<pattern_t>(std::move(v));
            }
        };

    private:
        /// \note just an example how to hide builder arguments validation.
        static auto validate(const type& v) -> void;
    };

    struct severity_t {
        using tag = required_tag<1>;
        using type = int;

        template<class Parent>
        struct setter {
            auto severity(type v) {
                return static_cast<Parent*>(this)->template set<severity_t>(std::move(v));
            }
        };
    };

    struct timestamp_t {
        using tag = optional_tag<1>;
        using type = long long;

        template<class Parent>
        struct setter {
            auto timestamp(type v) {
                return static_cast<Parent*>(this)->template set<timestamp_t>(std::move(v));
            }
        };
    };

    using result_type = std::unique_ptr<logger_t>;

    using component_tuple = std::tuple<
        pattern_t,
        severity_t,
        timestamp_t
    >;
};

struct logger_builder_t : public builder<logger_builder_t> {
private:
    /// \note hidden.
    auto complete(std::string pattern, int severity, std::optional<long long> timestamp) ->
        std::unique_ptr<logger_t> override;
};


int main() {
    // Won't compile, because severity component is required.
    //auto logger = logger_builder_t()
    //    .pattern("{}")
    //    .build();

    // Compiles perfectly even without optional timestamp component.
    auto logger = logger_builder_t()
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
