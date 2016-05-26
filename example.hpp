#pragma once

#include <string>

#include <mxbuilder/builder.hpp>

struct pattern_component {
    using tag = required_tag;
    using type = std::string;

    template<class Parent, class T>
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

    template<class Parent, class T>
    struct setter {
        auto severity(type v) {
            return static_cast<Parent*>(this)->template set<severity_component>(std::move(v));
        }
    };
};

struct timestamp_component {
    using tag = optional_tag;
    using type = long long;

    template<class Parent, class T>
    struct setter {
        auto timestamp(type v) {
            return static_cast<Parent*>(this)->template set<timestamp_component>(std::move(v));
        }
    };
};

struct logger_t {
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
    /// \note hidden.
    auto complete(std::string pattern, int severity, std::optional<long long> timestamp) ->
        std::unique_ptr<logger_t> override;
};
