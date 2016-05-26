#include <tuple>
#include <type_traits>
#include <ostream>
#include <experimental/tuple>
#include <experimental/optional>

#define __mxbuilder_static_check

namespace std {

using std::experimental::apply;
using std::experimental::optional;

}  // namespace std

/// Forwards.
template<class R, class... C>
struct builder;

namespace mxbuilder {
inline namespace v1 {

template<class T, class U>
struct __cat;

template<typename... T, typename... U>
struct __cat<std::tuple<T...>, std::tuple<U...>> {
    typedef std::tuple<T..., U...> type;
};

}  // namespace v1
}  // namespace mxbuilder

using namespace mxbuilder;

using required_tag = std::true_type;
using optional_tag = std::false_type;

using complete_tag = std::true_type;
using incomplete_tag = std::false_type;

template<class Tuple>
struct all_optional;

template<class... T>
struct all_optional<std::tuple<T...>> :
    public std::conditional<
        (... && std::is_same<T, optional_tag>::value),
        std::true_type,
        std::false_type
    >::type
{};

template<class Tuple>
struct is_complete;

template<class... Args>
struct is_complete<std::tuple<Args...>> {
    typedef typename all_optional<std::tuple<typename Args::tag...>>::type type;
};

template<class B, class R, class ComponentTuple, class = typename is_complete<ComponentTuple>::type>
struct state;

#ifdef __mxbuilder_static_check

static_assert(all_optional<std::tuple<>>::value);
static_assert(all_optional<std::tuple<optional_tag>>::value);
static_assert(all_optional<std::tuple<optional_tag, optional_tag>>::value);
static_assert(!all_optional<std::tuple<required_tag, optional_tag>>::value);

#endif

template<class T, class Tuple>
struct contains;

template<class T, class... U>
struct contains<T, std::tuple<U...>> :
    public std::conditional<
        (... || std::is_same<T, U>::value),
        std::true_type,
        std::false_type
    >::type
{};

static_assert(contains<bool, std::tuple<bool>>::value);
static_assert(contains<bool, std::tuple<bool, int>>::value);
static_assert(contains<bool, std::tuple<int, bool>>::value);
static_assert(!contains<bool, std::tuple<>>::value);

template<class... T>
struct unique {
    // for each T, check T... - T not contains T.
};

// static_assert(unique<bool, std::tuple<>>::value);
// static_assert(unique<bool, std::tuple<bool>>::value);
// static_assert(unique<bool, std::tuple<bool, int>>::value);
// static_assert(!unique<bool, std::tuple<bool, bool>>::value);


template<template<typename> class F, class Sequence>
struct filter;

template<template<typename> class F>
struct filter<F, std::tuple<>> {
    typedef std::tuple<> type;
};

template<template<typename> class F, typename T, typename... Args>
struct filter<F, std::tuple<T, Args...>> {
    typedef typename std::conditional<
        F<T>::value,
        typename __cat<std::tuple<T>, typename filter<F, std::tuple<Args...>>::type>::type,
        typename filter<F, std::tuple<Args...>>::type
    >::type type;
};

template<class Tuple, class T>
struct erase {
    template<class U>
    struct F {
        constexpr static auto value = !std::is_same<T, U>::value;
    };

    typedef typename filter<F, Tuple>::type type;
};

template<class B, class R, class T, class... C>
struct switch_state {
    typedef R result_type;
    typedef std::tuple<C...> tuple_type;

    typedef state<B, result_type, typename erase<tuple_type, T>::type> type;
};

template<class C, class T = typename C::type, class Tag = typename C::tag>
struct __storage_traits;

template<class C, class T>
struct __storage_traits<C, T, required_tag> {
    typedef T arg_type;
    typedef std::optional<T> storage_type;

    static auto unpack(storage_type& arg) noexcept -> arg_type& {
        return *arg;
    }
};

template<class C, class T>
struct __storage_traits<C, T, optional_tag> {
    typedef std::optional<T> arg_type;
    typedef std::optional<T> storage_type;

    static auto unpack(storage_type& arg) noexcept -> arg_type& {
        return arg;
    }
};

template<class B, class R, class... C>
struct state_common : public C... {
    B& builder;

    constexpr state_common(B& b) noexcept : builder(b) {}

    template<class U>
    auto set(U value) -> typename switch_state<B, R, U, C...>::type {
        std::get<typename __storage_traits<U>::storage_type>(builder.storage) = std::move(value).value;
        return {builder};
    }
};

template<class B, class R, class... C>
struct state<B, R, std::tuple<C...>, complete_tag> :
    public state_common<B, R, C...>
{
    using state_common<B, R, C...>::state_common;

    auto build() && -> R {
        return this->builder.do_build(this->builder.args());
    }
};

template<class B, class R, class... C>
struct state<B, R, std::tuple<C...>, incomplete_tag> :
    public state_common<B, R, C...>
{
    using state_common<B, R, C...>::state_common;
};

/// \tparam R builder result type.
/// \tparam Component component variadic pack.
template<class R, class... C>
struct builder :
    public state<builder<R, C...>, R, std::tuple<C...>>
{
public:
    // TODO: Check C... are unique.
    typedef R result_type;
    typedef std::tuple<typename __storage_traits<C>::arg_type...> tuple_type;
    typedef std::tuple<typename __storage_traits<C>::storage_type...> storage_type;

private:
    template<class, class, class, class> friend class state;
    template<class, class, class...> friend class state_common;

    storage_type storage;

public:
    constexpr builder() noexcept :
        state<builder<R, C...>, R, std::tuple<C...>>{*this}
    {}

private:
    virtual auto do_build(tuple_type args) -> result_type = 0;

    auto args() const -> tuple_type {
        return std::apply([&](auto... pack) {
            return std::make_tuple(__storage_traits<C>::unpack(pack)...);
        }, storage);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

struct pattern_component {
    typedef required_tag tag;
    typedef std::string type;

    type value;
};

struct severity_component {
    typedef required_tag tag;
    typedef int type;

    type value;
};

struct timestamp_component {
    typedef optional_tag tag;
    typedef long long type;

    type value;
};

struct result_t {
    std::string value;
};

template<class T>
void print(const T& v) {
    std::cout << v;
}

void print(const std::string& v) {
    std::cout << "\"" << v << "\"";
}

template<class T>
void print(const std::optional<T>& v) {
    if (v) {
        std::cout << "Some(" << *v << ")";
    } else {
        std::cout << "None";
    }
}

struct builder_t :
    public builder<
        result_t,
        pattern_component,
        severity_component,
        timestamp_component
    >
{
private:
    /// \note can be hidden.
    virtual auto do_build(std::tuple<std::string, int, std::optional<long long>> args) -> result_t override {
        std::apply([&](auto... args) {
            bool f = true;
            std::cout << "Tuple(";
            ((std::cout << (f ? "" : ", "), print(args), f = false),...);
            std::cout << ")\n";
        }, args);

        return result_t{""};
    }
};

int main() {
    auto result = builder_t()
        .set(pattern_component{"{}"})
        .set(severity_component{42})
        .build();

    return 0;
}

