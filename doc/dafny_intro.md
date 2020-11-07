# Dafny简介

Dafny中对应于一般程序设计语言的概念是method，function另有他用。

对于一个method，可以为它标注precondition和postcondition，为它保证的条件和它保证完成的行为。验证时要基于precondition证明postcondition

```dafny
method MakeBigger(a: int, b: int) returns (res: int)
    requires b > 0
    ensures a < res
{
    res := a + b;
}
```

如果把requires那句去掉，`a < res`就不能保证满足了，这时候就会报错

```plain
Dafny 2.3.0.10506
max.dfy(3,0): Error BP5003: A postcondition might not hold on this return path.
max.dfy(2,14): Related location: This is the postcondition that might not hold.
Execution trace:
    (0,0): anon0

Dafny program verifier finished with 4 verified, 1 error
```

调用一个函数时，要能够证明出来满足它的precondition，并可能会需要它的postcondition来证明调用者的行为。如果postcondition过弱，证不出来，也是没法通过验证的。下面的例子里，`Max3`的postcondition的证明是需要用到`Max`的postcondition的

```dafny
method Max(a: nat, b: nat) returns (res: nat)
    ensures res >= a && res >= b && (res == a || res == b)
{
    if a < b { res := b; }
    else { res := a; }
}
method Max3(a: nat, b: nat, c: nat) returns (res: nat)
    ensures res >= a && res >= b && res >= c && (res == a || res == b || res == c)
{
    var t := Max(a, b);
    res := Max(t, c);
}
```

如果把`Max`的`ensures`删掉，会看到`Max3`的postcondition不能证明出来，这里不贴信息了。

function是用来辅助说明spec的。写循环时需要标注循环不变式，它需要足够强来归纳证明循环不变式，以及在证明中表示这个循环的行为。

下面这个例子展示了这两个语法，同时也用到了数组——Dafny是有数组的

```dafny
function sum(arr: array<int>, n: nat): int
    reads arr
    requires n <= arr.Length
{
  if n == 0 then 0 else sum(arr, n - 1) + arr[n - 1]
}
method Sum(arr: array<int>) returns (res: int)
    ensures res == sum(arr, arr.Length)
{
    var i, s := 0, 0;
    while i < arr.Length
        invariant 0 <= i <= arr.Length
        invariant s == sum(arr, i)
    {
        s := s + arr[i];
        i := i + 1;
    }
    return s;
}
```

`sum`这个function用来在spec表示数组的前缀和，循环不变式保证了循环中的访问是合法的，以及`s`始终是已经扫过的部分的和。我还写了一个更复杂的例子，用于求数组的最大值，放在仓库的`code/dafny_example/max.dfy`中了。