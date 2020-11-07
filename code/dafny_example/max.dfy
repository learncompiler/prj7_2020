method Max(a: nat, b: nat) returns (res: nat)
    ensures res >= a && res >= b && (res == a || res == b)
{
    if a < b {
      res := b;
    } else {
      res := a;
    }
}

method Max3(a: nat, b: nat, c: nat) returns (res: nat)
    ensures res >= a && res >= b && res >= c && (res == a || res == b || res == c)
{
    var t := Max(a, b);
    res := Max(t, c);
}

method MaxArr(arr: array<int>) returns (res: int)
    requires forall k :: 0 <= k < arr.Length ==> arr[k] >= 0
    ensures forall k :: 0 <= k < arr.Length ==> arr[k] <= res
    ensures arr.Length > 0 ==> exists k :: 0 <= k < arr.Length && arr[k] == res
    ensures arr.Length == 0 ==> res == -1
{
    var i, s := 0, -1;
    while i < arr.Length
        invariant 0 <= i <= arr.Length
        invariant forall k :: 0 <= k < i ==> arr[k] <= s
        invariant i > 0 ==> exists k :: 0 <= k < i && arr[k] == s
        invariant i == 0 ==> s == -1
    {
        if arr[i] > s {
            s := arr[i];
        }
        i := i + 1;
    }
    return s;
}