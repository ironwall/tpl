#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_
#include <type_traits>

namespace tpl{

template<
    template<class ...> class A,
    template<class ...> class B
>
struct is_samett : std::false_type{};
template<
    template<class ...> class A
>
struct is_samett<A, A> : std::true_type{};

template<
    class A, class B
>
struct is_samett2 : std::false_type{};
template<
    template<class a> class A, class a,
    template<class b> class B, class b
>
struct is_samett2<A<a>, B<b>> : is_samett<A, B>{};

// template<class...Ts>
// struct all_same : std::false_type{};
// template<template<class v> class T, class v>
// struct all_same<T<v>> : std::true_type{};
// template<
//     template<class a> class T, class a, class b
// >
// struct all_same<T<a>, T<b>> : std::true_type{};
// template<class T1, class T2, class...Ts>
// struct all_same<T1, T2, Ts...> : all_same<T2, Ts...>{};


}
#endif