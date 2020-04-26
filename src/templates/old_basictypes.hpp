#ifndef BASIC_TYPES_HPP_
#define BASIC_TTPES_HPP_
#include <type_traits>
namespace tpl{

//wait for concepts
template<int i>
struct _INT_{};
template<class T>
struct _is_INT_ : std::false_type{};
template<
    template<int i> class T, int i
>
struct _is_INT_<T<i>> : std::true_type{};

template<class a, class = std::enable_if_t<_is_INT_<a>::value>>
struct Int{};



template<char c>
struct Char{};

template<int I, int D>
struct Float{};

template<bool b>
struct Bool{};

template<class T>
struct Just{};

}
#endif