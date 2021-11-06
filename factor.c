factor(N)
  P = empty queue
  Q = empty queue
  Q.enqueue(N)
  while Q is not empty
    n = Q.dequeue
    if n is prime
        P.enqueue(n)
        next loop
    d = find_divisor(n)
    Q.enqueue(d)
    Q.enqueue(n / d)
  output P

find_divisor(N)
  f = trial_division(N)
  if f != null
    return f
  if N <= 3037000499
    return pollard_rho(N)
  return squfof(N)