#include "example.hpp"

#include <iostream>
#include <string>

template<class T>
auto operator<<(std::ostream& stream, const std::optional<T>& v) -> std::ostream& {
    if (v) {
        return stream << "Some(" << *v << ")";
    } else {
        return stream << "None";
    }
}

auto pattern_component::validate(const type& v) -> void {
    if (v.empty()) {
        throw std::invalid_argument("pattern must not be empty");
    }
}

struct super_logger_t : public logger_t {
    std::string pattern;
    int severity;

    super_logger_t(std::string pattern, int severity) :
        pattern(std::move(pattern)),
        severity(severity)
    {}

    auto name() -> std::string {
        return "Logger(pattern=\"" + pattern + "\", severity=" + std::to_string(severity) + ")";
    }
};

auto builder_t::complete(std::string pattern, int severity, std::optional<long long> timestamp) ->
    std::unique_ptr<logger_t>
{
    std::cout << "R(\"" << pattern << "\", " << severity << ", " << timestamp << ")" << std::endl;
    return std::make_unique<super_logger_t>(std::move(pattern), severity);
}
