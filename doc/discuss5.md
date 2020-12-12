# 本周工作进展

本周做了代码实现，主要内容有：

+ 检查器的基本框架和作用域、符号表，这部分只是朴素的工程实现
+ 表达式树的轮子
+ 支持precondition和postcondition，用书上的方法来做verify

目前能够通过mdverif_testcases仓库里前两个step的测试，也就是能支持没有分支、没有assert、有precondition和postcondition的验证

verify的时候会顺便输出verification condition，比如说下面的代码

```cpp
// 用关键字 result 表示返回值
int mid(int l, int r)
    requires l >= 0
    requires l <= r
    ensures l <= result && result <= r
{
    return (l+r)/2;
}
```

是不能通过验证的，因为`l+r`可能溢出。输出如下：

```
vc: ((ns0::l) >= (0)) --> (((ns0::l) <= (ns0::r)) --> (((ns0::l) <= (((ns0::l) + (ns0::r)) / (2))) && ((((ns0::l) + (ns0::r)) / (2)) <= (ns0::r))))
Verification failed in a basic path in function mid_bad
Verification failed.
```

把`(l+r)/2`换成`l+(r-l)/2`后可以通过验证，输出如下：

```
vc: ((ns0::l) >= (0)) --> (((ns0::l) <= (ns0::r)) --> (((ns0::l) <= ((ns0::l) + (((ns0::r) - (ns0::l)) / (2)))) && (((ns0::l) + (((ns0::r) - (ns0::l)) / (2))) <= (ns0::r))))
Verified.
```

后续可能要做的：

+ if
+ 循环
+ assert
+ 函数调用

其中为了支持循环和函数调用，还需要添加decrease标注和相关的证明