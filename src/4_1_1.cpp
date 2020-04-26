#include <iostream>
#include <cxxabi.h>
#include <array>
using namespace std;

template<int>
struct check{};

struct X
{
    const static int n = 1;
    const static int m{2}; // C++11 起
    const static int k;
    //const static std::array<int, 1> arr2 = {1};
    const int arr[3] = { 4, 5, 6 }; 
    X(){
        check<arr[3]>{};
    }
};
const int X::k = 3;


int main(void) {
    const int MIN_SIZE = 996;
    const int MIN_SIZE_2 = MIN_SIZE + 1;
    const static int arr[] = { 4, 5, 6 }; 


    check<MIN_SIZE>{};
    check<MIN_SIZE_2>{};
    check<X::n>{};
    check<X::m>{};
    check<X::k>{};
}