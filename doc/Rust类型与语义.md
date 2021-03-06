## 1 所有权

一个复杂类型的一个值可能管理着某些资源（典型地，一块堆上的内存）。在Rust中，一个值的所有权在一个时刻属于一个变量。例如

```rust
let a = String::from("abc");
let b = a;
```

`String`内部是管理了一段堆内存的。如果直接把它各个成员的值拷贝过去，就会在离开作用域时出现double free。实际上，这段Rust代码把`a`中原本的值的所有权转移给了`b`。之后就不再允许使用`a`，因为它里面的值已经被move走了。离开作用域时，也只有`b`释放资源。

注意这个检查是编译期可以静态做的。

类似的还有传进签名为`fn f(s: String)`的函数时，也发生了所有权的转移。

例如智能指针`Box<T>`就会获取其管理的对象的所有权。

## 2 引用

引用并不转移所有权，只是"borrow"。对于同一个数据，Rust允许创建多个只读的引用，但如果存在可写的引用（`&mut`），就只允许这一个引用。这样类型的语义对程序员非常明确：操作一个引用时，如果是只读的引用，它的数据就不会在别的什么地方被更改。如果是可写的引用，它就只会被自己用这个引用改变。

为了运行时的安全，一个数据的引用不能活得比它自己长。为了完成这一点的检查，程序员可能需要对引用的生命周期进行标注来提示编译器，编译器检查这些标注确实不会被违反，以及这些标注能够保证运行时安全。

## 3 Copy trait

注意到像是`i32`这样的基本数据类型在`let b = a;`后`a`还是可以使用的，也就是说这里`a`并没有被移走，而是复制了。这是因为它们的类型具有`Copy` trait。这个trait在语义上表示这个类型并不管理资源，可以直接复制值。关于这里的语义问题，在网上发现了一些争议，主要集中在具有`Copy` trait的类型的`clone()`是否必须按位复制。[GitHub issue](https://github.com/ZhangHanDong/tao-of-rust-codes/issues/75)

在直觉上来说，如果一个类型是平凡的，只拥有一些基本数据或者其他平凡的类型作为成员，那么它可以拥有`Copy` trait