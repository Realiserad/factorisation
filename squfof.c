squfof(N, k)
  i = 0;
  kN = N * k // big integer computation
  P0 = isqrt(kN) // P_{i-1}
  P1 = 0 // P_{i}
  sqrtKN = P0; // floor(sqrt(kN))
  Q0 = 1; // Q_{i-1}
  Q1 = kN - P0^2 // Q_{i} // big integer computation
  Q2; // Q_{i+1}
  do
    b = (sqrtKN + P0) / Q1; // b_{i}
    P1 = (b * Q1) - P0;
    Q2 = Q0 + b * (P0 - P1);
    Q0 = Q1;
    Q1 = Q2;
    P0 = P1;
    if i++ is even && Q1 is perfect square
      break
  while true
  
  sqrtQ = isqrt(Q1)
  b0 = (sqrtKN - P0) / sqrtQ;
  P0 = (b0 * sqrtQ) + P0;
  Q0 = sqrtQ;
  Q1 = (kN - P0^2) / Q0 // big integer computation
  do
    b = (sqrtKN + P0) / Q1; // b_{i}
    P1 = (b * Q1) - P0;
    Q2 = Q0 + b * (P0 - P1);
	if P1 = P0
      break
    Q0 = Q1;
    Q1 = Q2;
    P0 = P1;
  while true

  return gcd(N, P1) // big integer computation