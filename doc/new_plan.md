# 第二周交流：重新规划与当前进展

## 1 规划

由于相应的前置工作过于庞大，打算并不基于已有的Prusti或者verasco做，而是自己在一个简单的语法上实现一个原型。

要做的工作：

+ 调研Dafny，这是一个成熟的验证语言，定义了一套语法用来写程序和spec，编译器检查代码时，只有spec可以被成功证明才能通过编译。Dafny编译器可以把Dafny程序编译到.NET, Go, JS和Java。需要实现的功能与Dafny相似，但会简单得多（比如应该不会有数组），语法也不一样。
+ 学习Z3，这是一个定理证明器，可以用于检查可满足性，如果可满足的话，给出interpretation
+ 学习前置知识，助教推荐了*The Calculus of Computation*，需要读前6章
+ 定义一套语法，在上面实现一个功能类似于Dafny检查器的东西。助教建议直接用MiniDecaf step8的语法，避免重复做一些琐碎的无关工作。

## 2 进展

+ [调研了Dafny](https://github.com/learncompiler/prj7_2020/blob/main/doc/dafny_intro.md)
+ 书读到了2.4。这里面讲的命题逻辑和一阶逻辑比大一学的多了不少东西，主要围绕着判断可满足性展开。