#include <iostream>
#include <cxxabi.h>
#include <array>
using namespace std;

template<int>
struct check{};
// 4.1.1
/*
struct X
{
    const static int n = 1;
    const static int m{2}; // C++11 起
    const static int k;
    const int arr[3] = { 4, 5, 6 }; 
    const static int arr3[3]; 
    const static std::array<int, 4> arr2;
   
    X(){
        //check<arr[3]>{};
    }
    
};

const int X::k = 3;
const int arr3[3] = {1,2,3};
const std::array<int, 4> arr2 = {1,2,3,4};
*/
//-----------------------4.1.1-------------------------------

// 4.1.2
constexpr double div10(const int x){
    return static_cast<double>(x)/10.0;
}
constexpr int add42(const double x){
    return static_cast<int>((x + 42)*10);
}
//-------------------------------------------------------------


int main(void) {
    //-----------------------4.1.1-------------------------------
    /*
    const int MIN_SIZE = 996;
    const int MIN_SIZE_2 = MIN_SIZE + 1;
    const static int arr[] = { 4, 5, 6 }; 


    check<MIN_SIZE>{};
    check<MIN_SIZE_2>{};
    check<X::n>{};
    check<X::m>{};
    check<X::k>{};
    */
    //-----------------------------------------------------------

    //-----------------------4.1.2-------------------------------
    constexpr int bleem = 34;
    constexpr int boom = add42(div10(bleem));
    int i;
    cin >> i;
    cout << div10(i);
    check<boom>{};
}