#pragma once

#include <string>

#include <mxbuilder/builder.hpp>

struct logger_t {
    virtual ~logger_t() = default;
    virtual auto name() -> std::string = 0;
};

struct builder_t;

template<>
struct builder_traits<builder_t> {
    struct pattern_t {
        using tag = required_tag;
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
        using tag = required_tag;
        using type = int;

        template<class Parent>
        struct setter {
            auto severity(type v) {
                return static_cast<Parent*>(this)->template set<severity_t>(std::move(v));
            }
        };
    };

    struct timestamp_t {
        using tag = optional_tag;
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

struct builder_t : public builder<builder_t> {
private:
    /// \note hidden.
    auto complete(std::string pattern, int severity, std::optional<long long> timestamp) ->
        std::unique_ptr<logger_t> override;
};
