#include <iostream>
#include "predicate.hpp"
#include "basictypes.hpp"
#include "list.hpp"
#include <cxxabi.h>
#include <type_traits>
using namespace std;
using namespace tpl;
template<class T>
constexpr void display(){
    T a;
    cout << abi::__cxa_demangle(typeid(a).name(), 0, 0, 0) << endl;
}

int main(){
    display<
        Int<_INT<2>>
    >();
    display<
        Int<Char<2>>
    >();
    display<enable_if<_is_INT<_INT<1>>::value, _INT<1>>>();
}
