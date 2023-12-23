# prj7_2020 Program Verification: mdverif

This is a study purpose formal verifier on a modified version of MiniDecaf (a C-like language for teaching purpose).

For detail syntax definition, see [mdverif.g4](generate/mdverif.g4).

Each function in the input program is split into basic paths. On each path, the verifier generates its verification condition (VC), a first-order formula representing the correctness of the path. The validity of the VCs is checked with Z3Prover. If all of them are valid, the verifier reports that the program is verified. Otherwise, it complains that an execution path is problematic.

## Usage

1. Install Z3 library. (see [their github repo](https://github.com/Z3Prover/z3)) Make sure that Z3 headers and the shared library `libz3.so` are in your build environment.

2. ```
   $ mkdir build
   $ cd build
   $ cmake ..
   $ make -j$(nproc)
   ```

3. `./mdverif <source code>`

## Examples

### Min

Input:

```C
int min(int a, int b)
    ensures result <= a && result <= b && (result == a || result == b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
```

Output:

```
vc: ((ns0::a) < (ns0::b)) --> ((((ns0::a) <= (ns0::a)) && ((ns0::a) <= (ns0::b))) && (((ns0::a) == (ns0::a)) || ((ns0::a) == (ns0::b))))
vc: (! ((ns0::a) < (ns0::b))) --> ((((ns0::b) <= (ns0::a)) && ((ns0::b) <= (ns0::b))) && (((ns0::b) == (ns0::a)) || ((ns0::b) == (ns0::b))))
```

### Bad Mid

Input:

```c
int mid(int l, int r)
    requires 0 <= l && l <= r
    ensures l <= result && result <= r && 0 <= (r - result) - (result - l) && (r - result) - (result - l) <= 1
{
    return (l + r) / 2;
}
```

Output:

```
vc: (((0) <= (ns0::l)) && ((ns0::l) <= (ns0::r))) --> (((((ns0::l) <= (((ns0::l) + (ns0::r)) / (2))) && ((((ns0::l) + (ns0::r)) / (2)) <= (ns0::r))) && ((0) <= (((ns0::r) - (((ns0::l) + (ns0::r)) / (2))) - ((((ns0::l) + (ns0::r)) / (2)) - (ns0::l))))) && ((((ns0::r) - (((ns0::l) + (ns0::r)) / (2))) - ((((ns0::l) + (ns0::r)) / (2)) - (ns0::l))) <= (1)))
Verification failed in a basic path in function mid
Verification failed.
```

### Good Mid

Input:

```c
int mid(int l, int r)
    requires 0 <= l && l <= r
    ensures l <= result && result <= r && 0 <= (r - result) - (result - l) && (r - result) - (result - l) <= 1
{
    return l + (r - l) / 2;
}
```

Output:

```
vc: (((0) <= (ns0::l)) && ((ns0::l) <= (ns0::r))) --> (((((ns0::l) <= ((ns0::l) + (((ns0::r) - (ns0::l)) / (2)))) && (((ns0::l) + (((ns0::r) - (ns0::l)) / (2))) <= (ns0::r))) && ((0) <= (((ns0::r) - ((ns0::l) + (((ns0::r) - (ns0::l)) / (2)))) - (((ns0::l) + (((ns0::r) - (ns0::l)) / (2))) - (ns0::l))))) && ((((ns0::r) - ((ns0::l) + (((ns0::r) - (ns0::l)) / (2)))) - (((ns0::l) + (((ns0::r) - (ns0::l)) / (2))) - (ns0::l))) <= (1)))
Verified.
```

### Sum

Input:

```c
int sum(int n)
    requires 0 <= n && n <= 46340
    ensures result == n * (n + 1) / 2
{
    int sum = 0;
    int i;
    for (i = 1; i <= n; i = i + 1)
        invariant 0 <= n && n <= 46340
        invariant 0 <= i && i <= n + 1 && sum == i * (i - 1) / 2
        decrease (n + 1 - i)
    {
        sum = sum + i;
    }
    return sum;
}
```

Output:

```
vc: (((0) <= (ns0::n)) && ((ns0::n) <= (46340))) --> ((((0) <= (ns0::n)) && ((ns0::n) <= (46340))) && (((((0) <= (1)) && ((1) <= ((ns0::n) + (1)))) && ((0) == (((1) * ((1) - (1))) / (2)))) && ((((ns0::n) + (1)) - (1)) >= (0))))
vc: (((0) <= (ns0::n)) && ((ns0::n) <= (46340))) --> (((((0) <= (ns0::i)) && ((ns0::i) <= ((ns0::n) + (1)))) && ((ns0::sum) == (((ns0::i) * ((ns0::i) - (1))) / (2)))) --> ((((ns0::i) <= (ns0::n)) --> ((((0) <= (ns0::n)) && ((ns0::n) <= (46340))) && (((((0) <= ((ns0::i) + (1))) && (((ns0::i) + (1)) <= ((ns0::n) + (1)))) && (((ns0::sum) + (ns0::i)) == ((((ns0::i) + (1)) * (((ns0::i) + (1)) - (1))) / (2)))) && (((((ns0::n) + (1)) - ((ns0::i) + (1))) >= (0)) && ((((ns0::n) + (1)) - ((ns0::i) + (1))) < (((ns0::n) + (1)) - (ns0::i))))))) && ((((ns0::n) + (1)) - (ns0::i)) >= (0))))
vc: (((0) <= (ns0::n)) && ((ns0::n) <= (46340))) --> (((((0) <= (ns0::i)) && ((ns0::i) <= ((ns0::n) + (1)))) && ((ns0::sum) == (((ns0::i) * ((ns0::i) - (1))) / (2)))) --> (((! ((ns0::i) <= (ns0::n))) --> ((ns0::sum) == (((ns0::n) * ((ns0::n) + (1))) / (2)))) && ((((ns0::n) + (1)) - (ns0::i)) >= (0))))
Verified.
```

