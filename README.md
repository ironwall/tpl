## 4 元编程下的数据结构与算法
### 4.1 表达式与数值计算
本小节会介绍const, constexpr，consteval和常量表达式（还有与static相关的情况）
#### 4.1.1 const与常量表达式
我们先从大家都熟悉的const开始来介绍常量表达式，总所周知const是用于修饰一个常量的，我们声明一个普通的const 常量就必须给予初始化。
而所谓的常量表达式，就是定义能在编译时求值的表达式。
比如：

```C++
//为了方便期间，我们用一个简单的类模板来观察一个表达式是否是常量表达式
template<int x>
struct check{};
const int MIN_SIZE = 996; check<MIN_SIZE>{};
		//OK, 且MAX_SIZE是常量表达式，check<MIN_SIZE>{}是通过编译
const int MIN_SIZE_2 = MIN_SIZE + 1; check<MIN_SIZE_2>{};
		//OK, MIN_SIZE_2也是常量表达式，也在编译期就已经确定
const int FOO = return_10();	//这个是OK的
check<FOO>{};	//Error,  FOO并非常量表达式，FOO在运行期确定。
/*假设我们的函数是这样的
int return_10(){
	return 10;
}
*/
```
函数的返回值是可以赋值给常量的，但是看起来如此简单的函数赋值给const常量，很明显一看就是可以在编译期间就可以得到结果的，但是实际上它变成了一个read only的变量（运行期），实际中我们可能会遇到一些传进来是常量表达式，而且输出也是简单固定的，那么把这些计算优化到编译期去做，显然会让程序变得稍微快那么一些，能让编译器去这么做的关键字在下面会介绍到。

此外，在类（class、struct）中，我们可以用以下几种形式：

```C++
//Referenced from: https://zh.cppreference.com/w/cpp/language/static
struct X
{
    static const int n = 1;
    static const int m{2}; // C++11 起
    static const int k;
    //static const int arr[] = { 4, 5, 6 };  出错，static const 不可以初始化复杂的类型 
    const int arr[3] = { 4, 5, 6 }; 	//但是类中可以定义const int arr[3] = { 4, 5, 6 }; 
    X(){
        //check<arr[3]>{}; 出错，this指针不能再常量表达式中使用
    }
};
const int X::k = 3;

//在main函数中的测试:
check<X::n>{};		//OK
check<X::m>{};		//OK
check<X::k>{};		//OK
```
static const 在类中并不支持初始化复杂的类型，但是我们明眼人仍旧可以看出这里的`static const int arr[] = { 4, 5, 6 }`也完全是可以在编译期就搞定的东西。甚至像是`static const std::array<int, 4> arr2;` 这样的也明显可以是在编译期就搞定的，那么就引出我们的新生代主角constexpr

#### 4.1.2 constexpr与常量表达式
从C++11标准起，引入了一个关键字叫做constexpr，顾名思义，就是const expression，就是常量表达式。
constexpr可以修饰函数，变量等等（不可以修饰函数的参数），对于被声明为constexpr的变量或者函数，则它们在编译期间被求值是**可能的**（也就是也可以不能）
现在我们来思考这样一个情景，假设我们用常量表达式定义了一个编译期整数常量`constexpr int bleem  = 34;`据说这是一个非常强大的数，我们定义了这个数，通过一系列很nb轰轰的函数就可以得到另一个更为强大的整数，通过这个整数我们可以爆炸了。好了，我们要定义一个新的数如果仅仅使用手写会很不好看，显然定义成函数调用的形式就非常合适，那么就是`constexpr double boom = add42(div10(bleem))`; 

```C++
constexpr double div10(const int x){
    return static_cast<double>(x)/10.0;
}
constexpr int add42(const double x){
    return static_cast<int>((x + 42)*10);
}
int main(void){
    constexpr int bleem = 34;
    constexpr int boom = add42(div10(bleem));
    check<boom>{};
}
```
没有问题，我们成功得到了可以爆炸的数。我们可以看到，当传入的实参和函数都是常量表达式时，我们的嵌套调用的计算函数确实是可以在编译期进行运算的。
另外我们需要注意的一点是，constexpr的函数是需要非常简单的，而且还需要的一些其他要求，如果要做到constexpr函数在编译期运算。需要满足很多条件：
其中最显然的有：

* 其返回类型（若存在）必须是字面类型 (LiteralType)
* 其每个参数都必须是字面类型 (LiteralType)

另外因为我们的constexpr函数的参数没法指定为constexpr，所以我们无法写像下面这样的代码
```C++
constexpr double div10(const int x){
    constexpr int xcopy = x; 		//x并没有被声明为constexpr常量
    return x/10;
}
```

更多详细的条例可以在这里找到参考 [constexpr 说明符](https://zh.cppreference.com/w/cpp/language/constexpr)

前面提到过，在类里面，static const 不可以初始化复杂的类型 ，但是constexpr可以:

```C++
//Referenced from: https://zh.cppreference.com/w/cpp/language/static
struct Y {
    static constexpr int arr[] = { 1, 2, 3 };        // OK
    static constexpr std::complex<double> n = {1,2}; // OK
    static constexpr int k; // 错误：constexpr static 要求初始化器
};
```
有时候我们看着const和constexpr是有点类似的东西，例如上面的X与Y两个类，constexpr可以初始化的对象比const要复杂一点，而编译器还允许我们同时使用constexpr和const，比如`constexpr const N = 999;`，但是在这里constexpr包含了const的语义，在这里我们可以写成`constexpr N = 999` 或者 `const N = 999`，从c++11起，constexpr对于类成员函数也包含了const的语义，即不改变类的成员对象。
还要一点需要注意的是，对于constexpr函数，虽然我们显式地写出来告诉编译期这个函数是可以在编译期计算的，但是它也可以不是在编译期运行的函数。比如我们之前所写的函数

```C++
constexpr double div10(const int x){
    return static_cast<double>(x)/10.0;
}
constexpr int add42(const double x){
    return static_cast<int>((x + 42)*10);
}

int i;
cin >> i;
div10(i);			//我们可以当成普通的运行期函数调用，但是返回值已经不再是编译期常量了
```
从C++17开始，constexpr也可以用于lambda表达式了，而且如果你不显式地把constexpr写上而lambda函数的写法完全符合constexpr函数时，它也会是一个constexpr函数。

#### 4.1.3 consteval与常量表达式
C++20开始，我们有了一个新的说明符consteval，consteval - 指定函数是立即函数（immediate function），即每次调用该函数必须产生编译时常量。(Referenced from cppreference)
constexpr还有编译期和运行期的两面性，但consteval则完全都是编译期了。

#### 4.1.2 百玩不厌的老梗：第n个斐波那契数
//To be continue

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
template<typename Head, typename Rest>
struct Cons{};
template<typename  T>
struct Cons<T, Nil>{}; //别忘了这个
```
我们可以用`typeid(Cons<Nil, Cons<Nil, Nil> >{}).name() //(gcc需要使用abi::__cxa_demangle(...))`来看一下我们得到了什么:`Cons<Nil, Cons<Nil, Nil> >`就是这个，没问题。
我们知道C++ template是支持可变模板参数的，所以我们还可以定义一个List，然后把List转换成Cons结构。

```C++
template<typename head, typename ...tail>
struct make_list {
    using result = Cons<head, typename make_list<tail...>::result>;
};
template<typename head>
struct make_list<head> {
    using result = Cons<head, Nil>;
};
template<typename ...arg>
using List = typename make_list<arg...>::result;
```
咦？那么既然支持形参包，那么我们完全可以不用Cons结构，直接造出一个List来：

```C++
template<typename...>
struct List{};
```
干干净净，甚至什么都没有。虽然`Cons`结构在函数式编程语言里是更为"核心"的存在，但是我们没有必要照本宣科，如此定义的List简洁好看，也一样方便使用，而且我们还可以反过来，把`Cons`结构变成我们的`List`，上面的`make_list`这样看起来恶心的一坨就没有必要存在了。
首先我们需要定义一个concat函数来合并两个List：

```C++
template<typename, typename>
struct concat{};
template<typename ...items1, typename ...items2>
struct concat<List<items1...>, List<items2...>>{
    using result = List<items1..., items2...>;
};
template<typename list1, typename list2>
using concat_r = typename concat<list1, list2>::result;
```
当我们编写这种“函数”的时候，便于思考的形式就是先写下函数的原型，比如`template<typename list, typename list2>
struct concat{};`先声明了参数是两个List，然后具体的函数是怎么操作的，我们再利用特化和偏特化来处理。下面的写法大多都是基于这种形式的。

现在我们拥有了这个无比强大的函数，我们就可以把`Cons<...>`转化为List了：

```C++
template<typename head, typename rest>
struct Cons{
    using toList = typename concat< List<head>, typename rest::toList> :: result;
};
template<typename head>
struct Cons<head, Nil>{
    using toList = List<head>;
};
```
这么写有点不好看，我们在Cons里面挖了一个洞，这里的toList可以看做一个函数，这样还颇有一种面向对象的味道。当然我们也可以保持Cons的纯洁性(即不写上面那种)，而去增加一个函数进行转化：

```C++
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
```
为了方便我们的测试，我们可以定义一个数值类型Int：

```C++
template<int>
struct Int{};
```
我们尝试运行`listfromCons_r<Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil> > > >`，得到了`List<Int<1>, Int<1>, Int<2> >`，尝试`Cons<Int<1>, Cons<Int<1>, Cons<Int<2>, Nil> > >::toList`，也得到了一样的值。

然后反过来也是一样的:

```C++
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
```
`consfromList_r<List<Int<123>, Int<32>, Int<1> > >` 变成了
`Cons<Int<123>, Cons<Int<32>, Cons<Int<1>, Nil> > >`

#### 4.2.2 也是Tuple？List上的类型限定

我们思考一下Tuple会写成什么样子，我们的Tuple里的类型显然是“泛型”的，也就是什么类型都可以往里面塞，也肯定是支持可变长参数的，所以 我们会写成。。。

```C++
template<typename...>
struct Tuple{};
```
让我们仔细再观察一下List的写法

```C++
template<typename...>
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
template<template<typename...> typename T, template<typename...> typename U>
struct is_same_template_name : std::false_type{};
template<template<typename...> typename T>
struct is_same_template_name<T, T> : std::true_type{};
```
我们看看，`is_same_template_name`接受的参数是什么呢？显然是一个“带模板的模板”，所以我们的`is_same_template_name`可以这么调用
`is_same_template_name<Int, Char>`。但是这里有一个问题，我们这个接受的模板，它本身的参数是typename，不能是int和char啊，那么我们真的可以这么调用吗？显然不行，因为我们注意到，此时的`Int<?>` 中的 ? 是 int类型的，那么显然我们需要重写我们的基本类型：

```C++
template<int>
struct _Int_{};
template<typename>
struct Int{};

template<char>
struct _Char_{};
template<typename>
struct Char{};
```
这样我们把`Int`和`Char`的参数也变成了typename，可以说非常繁琐了，但是得到的效果是可喜可贺的，我们现在看看`is_same_template_name<Int, Char>::type`，我们得到了`std::integral_constant<bool, false>`。
接下来我们尝试写出这个`is_all_same_type`:

```C++
template<typename...T>
struct is_all_same_type : std::false_type{};
template< 
    template<typename...t> typename T, typename...t,
    template<typename...u> typename U, typename...u
>
struct is_all_same_type<T<t...>, U<u...>> : is_same_template_name<T, U>{}; 
template<typename T1, typename T2, typename...R>
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
template<typename...Ts>
struct StrictList : List<std::enable_if_t<is_all_same_type<Ts...>::value>>{};
```

 我们尝试使用`StrictList<Int(1), Char(2)>` No，报错了，不可以，而` StrictList<Int(1), Int(2)>{}` 可以


### 4.3 字典结构
### 4.4 “快速”排序
### 4.5 其它常用的“轮子”
