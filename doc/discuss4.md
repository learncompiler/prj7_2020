# 本周工作进展

1. 学习了一下Z3的API。Z3提供了C++ API，接口比较直观。上次说的构造出来的verification condition取反后用Z3判断可满足性，就可以说明VC是否永真。可满足时有个`.get_model()`能提供一个使其满足的interpretation，这个后续应该用不到，不过可以用来做一些有意思的事。

   ```cpp
   #include "z3++.h"
   
   using namespace z3;
   int main()
   {
       context c;
       expr x = c.int_const("x"), y = c.int_const("y");
       expr equations = ((x + y == 3) && (x - y == 1));
       solver s(c);
       s.add(equations);
       assert(s.check() == sat);
       model m = s.get_model();
       std::cout << m.eval(x) << ' ' << m.eval(y) << '\n';
   }
   ```

2. 开始代码实现，从MiniDecaf实验拿过来`third_party`和CMake配置，语法用step8的语法稍微改了改（要加上`requires`，`ensures`，`-->`，后续应该还要继续加，比如说assert，量词）
   助教帮我写了一些测例，并且做了开发步骤的划分，下面要做的是让它能够处理直接的`ensures`，也就是能把这个语法里写的表达式转换成Z3的。

