#ifndef LIST_HPP_
#define LIST_HPP_

namespace tpl{

template<class...>
struct mList{};

// template<class...>
// struct List{};
// template<class a, class...T>
// struct List<>{

// };

template<class...>
struct List{};

template<class list, class list2>
struct concat{};
template<class ...items1, class ...items2>
struct concat<List<items1...>, List<items2...>>{
    using result = List<items1..., items2...>;
};
template<class list1, class list2>
using concat_r = typename concat<list1, list2>::result;

struct Nil{};
template<class head, class rest>
struct Cons{
    using toList = typename concat< List<head>, typename rest::toList> :: result;
};
template<class head>
struct Cons<head, Nil>{
    using toList = List<head>;
};

template<class T>
struct listfromCons{};
template<class head, class rest>
struct listfromCons<Cons<head, rest>>{
    using result = typename concat< 
        List<head>, 
        typename listfromCons<rest>::result 
    >::result;
};
template<class head>
struct listfromCons<Cons<head, Nil>>{
    using result = List<head>;
};
template<class T>
using listfromCons_r = typename listfromCons<T>::result;


}
#endif