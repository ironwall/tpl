#include <iostream>
#include <cxxabi.h>
#include <type_traits>
#include <array>
using std::cout;
using std::endl;

template<int>
struct _Int_{};
template<typename>
struct Int{};

template<char>
struct _Char_{};
template<typename>
struct Char{};


template<typename...>
struct List{};

template<typename , typename>
struct concat{};
template<typename ...items1, typename ...items2>
struct concat<List<items1...>, List<items2...>>{
    using result = List<items1..., items2...>;
};
template<typename list1, typename list2>
using concat_r = typename concat<list1, list2>::result;


//首先我们定义一个Nil
struct Nil{};
template<typename head, typename rest>
struct Cons{
    using toList = typename concat< List<head>, typename rest::toList> :: result;
};
template<typename head>
struct Cons<head, Nil>{
    using toList = List<head>;
};

template<typename>
struct listfromCons{};
template<typename head, typename rest>
struct listfromCons<Cons<head, rest>>{
    using result = typename concat< 
        List<head>, 
        typename listfromCons<rest>::result 
    >::result;
};
template<typename head>
struct listfromCons<Cons<head, Nil>>{
    using result = List<head>;
};
template<typename T>
using listfromCons_r = typename listfromCons<T>::result;

template<typename>
struct consfromList{};
template<typename head, typename...rest>
struct consfromList<List<head, rest...>>{
    using result = Cons<
        head, typename consfromList<List<rest...> >::result
    >;
};
template<typename tail>
struct consfromList<List<tail>>{
    using result = Cons<tail, Nil>;
};
template<typename T>
using consfromList_r = typename consfromList<T>::result;

template<template<typename...> typename T, template<typename...> typename U>
struct is_same_template_name : std::false_type{};
template<template<typename...> typename T>
struct is_same_template_name<T, T> : std::true_type{};

template<typename...>
struct is_all_same_type : std::false_type{};
template< 
    template<typename...t> typename T, typename...t,
    template<typename...u> typename U, typename...u
>
struct is_all_same_type<T<t...>, U<u...>> : is_same_template_name<T, U>{};
template<typename T1, typename T2, typename...R>
struct is_all_same_type<T1, T2, R...> : is_all_same_type<T2, R...>{};


template<typename...Ts>
struct StrictList : List<std::enable_if_t<is_all_same_type<Ts...>::value>>{};
#define Int(n) Int<_Int_<n>>
#define Char(n) Char<_Char_<n>>

template<int x>
constexpr size_t lengthForArray(){
    return 1;
}
template<int x, int y, int...rest>
constexpr size_t lengthForArray(){
    return 1 + lengthForArray<y, rest...>();
}

template<typename T, T...s>
struct Array1 : std::array<T, lengthForArray<s...>()>{
    constexpr Array1(): std::array<T, lengthForArray<s...>()>{s...}{}
    constexpr auto operator [](size_t pos) const{
        return this->_M_elems[pos];
    }
};

template<typename T, T...s>
constexpr auto Array2(){
    return std::array<T, lengthForArray<s...>()>{s...};
};

template<typename T, T...s>
struct Array3{
    const std::array<T, lengthForArray<s...>()> content{s...};
    constexpr auto operator [](size_t pos) const {
        return content[pos];
    }
};



int main(){
    /*
    cout << abi::__cxa_demangle(typeid(
        listfromCons_r<Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil>>>>{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil>>>::toList{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        consfromList_r<List<Int<123>, Int<32>, Int<1>>>{}
    ).name(), 0, 0, 0) << endl;
    */
    cout << abi::__cxa_demangle(typeid(
        is_same_template_name<Int, Char>::type{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        is_all_same_type<Int<_Int_<1>>, Char<_Char_<'c'>>>::type{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        is_all_same_type<Int<_Int_<1>>, Int<_Int_<2>>, Int<_Int_<3>>>::type{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        is_all_same_type<
            List<Int<_Int_<1>>>, List<Char<_Char_<'a'>>>
        >::type{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        StrictList<Int(1), Int(2)>{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        StrictList<List<Int(1)>, List<Int(2), Int(3)>>{}
    ).name(), 0, 0, 0) << endl;

    cout << lengthForArray<1,22,3,4,5>() << endl;
cout << "===========Array=========" << endl;
    cout << "Array1<int, 1,2,3,4> \t\t==> " << abi::__cxa_demangle(typeid(
        Array1<int, 1,2,3,4>{}
    ).name(), 0, 0, 0) << endl;
    cout << "Array1<int, 12,3,2,5,5>{}[1] \t==> "<< Array1<int, 12,3,2,5,5>{}[1] << endl;

    cout << "Array2<int, 1,2,3,4>()[3] \t==> " << 
        Array2<int, 1,2,3,4>()[3]
    << endl;

    cout << "Array3<int, 1,2,3,4>{} \t\t==> " << abi::__cxa_demangle(typeid(
        Array3<int, 1,2,3,4>{}
    ).name(), 0, 0, 0) << endl;
    cout << "Array3<int, 1,2,3,4>{}[2] \t==> " << 
        Array3<int, 1,2,3,4>{}[2]
    << endl;
cout << "===========_Int<Array?>=========" << endl;
    cout << abi::__cxa_demangle(typeid(
        _Int_<Array3<int, 12,3,2,5,5>{}[1]>{}
    ).name(), 0, 0, 0) << endl;
    cout << abi::__cxa_demangle(typeid(
        _Int_<Array1<int, 12,3,2,5,5>{}[1]>{}
    ).name(), 0, 0, 0) << endl;

}
