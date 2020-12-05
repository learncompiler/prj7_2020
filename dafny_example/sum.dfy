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