brent64(N)
  xi = random.nextLong // random start value
  xm = xi
  i = 1
  while true
    xi = (xi * xi) + 1 mod N // move rabbit one step
    d = gcd(xi - xm, N)
    if d != 1 && d != N
      return d
    if i is power of two
      xm = xi // teleport tortoise
    next i