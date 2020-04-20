## 4 元编程下的数据结构与算法
### 4.1 表达式与数值计算
#### 4.1.1 从求第n个斐波那契数开始
```C++
template<int x>
struct fib{
	enum{ 
		result = fib<x-1>::result + fib<x-2>::result
	};
};
template<>
struct fib<2>{
	enum{
		result = 1
	}; 
};
template<>
struct fib<1>{
	enum{
		result = 1
	};
};
```
尾递归:

```C++
template<int a, int b, int count>
struct fib_iter{
    enum{
        result = fib_iter<a+b, a, count-1>::result
    };
};
template<int a, int b>
struct fib_iter<a, b, 0>{
    enum{
        result = b
    };
};

template<int n>
struct fib{
    enum{
        result = fib_iter<1, 0, n>::result
    };
};
```

### 4.1 获得类型的属性——类型萃取（Type Traits） 
### 4.2 列表与数组
#### 4.2.1 再看List
首先我们来看一个Haskell中List的例子:

```Haskell
data MyList a = Nil | Cons a (MyList a)
```
那么根据同样思路，我们可以写成这样：

```C++
//首先我们定义一个Nil
struct Nil{};
//定义Cons：
template<class Head, class Rest>
struct Cons{};
template<class  T>
struct Cons<T, Nil>{}; //别忘了这个
```
我们可以用`typeid(Cons<Nil, Cons<Nil, Nil> >{}).name() //(gcc需要使用abi::__cxa_demangle(...))`来看一下我们得到了什么:`Cons<Nil, Cons<Nil, Nil> >`就是这个，没问题。
我们知道C++ template是支持可变模板参数的，所以我们还可以定义一个List，然后把List转换成Cons结构。

```C++
template<class head, class ...tail>
struct make_list {
    using result = Cons<head, typename make_list<tail...>::result>;
};
template<class head>
struct make_list<head> {
    using result = Cons<head, Nil>;
};
template<class ...arg>
using List = typename make_list<arg...>::result;
```
咦？那么既然支持形参包，那么我们完全可以不用Cons结构，直接造出一个List来：

```C++
template<class...>
struct List{};
```
干干净净，甚至什么都没有。虽然`Cons`结构在函数式编程语言里是更为"核心"的存在，但是我们没有必要照本宣科，如此定义的List简洁好看，也一样方便使用，而且我们还可以反过来，把`Cons`结构变成我们的`List`，上面的`make_list`这样看起来恶心的一坨就没有必要存在了。
首先我们需要定义一个concat函数来合并两个List：

```C++
template<class list1, class list2>
struct concat{};
template<class ...items1, class ...items2>
struct concat<List<items1...>, List<items2...>>{
    using result = List<items1..., items2...>;
};
template<class list1, class list2>
using concat_r = typename concat<list1, list2>::result;
```
当我们编写这种“函数”的时候，便于思考的形式就是先写下函数的原型，比如`template<class list, class list2>
struct concat{};`先声明了参数是两个List，然后具体的函数是怎么操作的，我们再利用特化和偏特化来处理。下面的写法大多都是基于这种形式的。

现在我们拥有了这个无比强大的函数，我们就可以把`Cons<...>`转化为List了：

```C++
template<class head, class rest>
struct Cons{
    using toList = typename concat< List<head>, typename rest::toList> :: result;
};
template<class head>
struct Cons<head, Nil>{
    using toList = List<head>;
};
```
这么写有点不好看，我们在Cons里面挖了一个洞，这里的toList可以看做一个函数，这样还颇有一种面向对象的味道。当然我们也可以保持Cons的纯洁性(即不写上面那种)，而去增加一个函数进行转化：

```C++
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
```
为了方便我们的测试，我们可以定义一个数值类型Int：

```C++
template<int>
struct Int{};
```
我们尝试运行`listfromCons_r<Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil> > > >`，得到了`List<Int<1>, Int<1>, Int<2> >`，尝试`Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil> > >::toList`，也得到了一样的值。

然后反过来也是一样的:

```C++
template<class T>
struct consfromList{};
template<class head, class...rest>
struct consfromList<List<head, rest...>>{
    using result = Cons<
        head, typename consfromList<List<rest...> >::result
    >;
};
template<class tail>
struct consfromList<List<tail>>{
    using result = Cons<tail, Nil>;
};
template<class T>
using consfromList_r = typename consfromList<T>::result;
```
`consfromList_r<List<Int<123>, Int<32>, Int<1> > >` 变成了
`Cons<Int<123>, Cons<Int<32>, Cons<Int<1>, Nil> > >`

#### 4.2.2 也是Tuple？List上的类型限定

我们思考一下Tuple会写成什么样子，我们的Tuple里的类型显然是“泛型”的，也就是什么类型都可以往里面塞，也肯定是支持可变长参数的，所以 我们会写成。。。

```C++
template<class...>
struct Tuple{};
```
让我们仔细再观察一下List的写法

```C++
template<class...>
struct List{};
```

emmm，变得和List完全一样了。我们发现这里的List是类似于Python里的list一样，我们可以传入任何类型的参数，比如我们再构造一个Char类型的数据

```C++
template<char>
struct Char{};
```
然后我们可以这么写List：`List<Int<1>, Char<'c'> >`

甚至可以做List的嵌套：`List<Int<1>, List<Int<1>, Int<2>>>`

那么我们是否可以对List里的类型做任何限定呢？我们需要List里的类型都是一样的，这样的List可以做到吗？

这里我们需要引入template template parameter这个东西了，我们先思考，如何判断多个个`Int<?>`是一样的, 即我们需要这样的一个可变长参数的的判断函数：

```
is_all_same_type<Int<1>, Int<2>, Int<3>> 	==> std::integral_constant<bool, true>
is_all_same_type<Int<1>, Char<'c'>>			==> std::integral_constant<bool, false>
```
我们可以用`is_same`吗，很遗憾，不可以，对于`is_same`来说，`Int<1>, Int<2>`是两个不一样的类型，那么怎么构造我们的`is_all_same_type`呢？
别那么着急，我们先一步一步来，既然我们要做到这一点，我们先从简单的两个类型开始，我们先定义一个`is_same_template_name`，因为很显然，`Int<1>`和`Int<2>`相同的点显然在这个`Int`。

```C++
template<template<class...> class T, template<class...> class U>
struct is_same_template_name : std::false_type{};
template<template<class...> class T>
struct is_same_template_name<T, T> : std::true_type{};
```
我们看看，`is_same_template_name`接受的参数是什么呢？显然是一个“带模板的模板”，所以我们的`is_same_template_name`可以这么调用
`is_same_template_name<Int, Char>`。但是这里有一个问题，我们这个接受的模板，它本身的参数是class，不能是int和char啊，那么我们真的可以这么调用吗？显然不行，因为我们注意到，此时的`Int<?>` 中的 ? 是 int类型的，那么显然我们需要重写我们的基本类型：

```C++
template<int>
struct _Int_{};
template<class>
struct Int{};

template<char>
struct _Char_{};
template<class>
struct Char{};
```
这样我们把`Int`和`Char`的参数也变成了class，可以说非常繁琐了，但是得到的效果是可喜可贺的，我们现在看看`is_same_template_name<Int, Char>::type`，我们得到了`std::integral_constant<bool, false>`。
接下来我们尝试写出这个`is_all_same_type`:

```C++
template<class...T>
struct is_all_same_type : std::false_type{};
template< 
    template<class...t> class T, class...t,
    template<class...u> class U, class...u
>
struct is_all_same_type<T<t...>, U<u...>> : is_same_template_name<T, U>{}; 
template<class T1, class T2, class...R>
struct is_all_same_type<T1, T2, R...> : is_all_same_type<T2, R...>{}; //这里可以用继承来获得{true/false}_type
```
我们来测试一下：

```C++
is_all_same_type<Int<_Int_<1>>, Char<_Char_<'c'>>>::type
  ==>  std::integral_constant<bool, false>
is_all_same_type<Int<_Int_<1>>, Int<_Int_<2>>, Int<_Int_<3>>>::type 
  ==> std::integral_constant<bool, true>
is_all_same_type<
    List<Int<_Int_<1>>>, List<Char<_Char_<'a'>>>
>::type										
  ==> std::integral_constant<bool, true>
```

觉得`Int<_Int_<2>>`太繁琐了吧，我们可以用宏来遮掩一下丑陋：

```C++
#define Int(n) Int<_Int_<n>>
#define Char(n) Char<_Char_<n>>
```
然后我们就可以定义我们的严格列表了：

```C++
template<class...Ts>
struct StrictList : List<std::enable_if_t<is_all_same_type<Ts...>::value>>{};
```

 `StrictList<Int(1), Char(2)>`不可以，` StrictList<Int(1), Int(2)>{}` 可以


### 4.3 字典结构
### 4.4 “快速”排序
### 4.5 其它常用的“轮子”
