import java.math.BigInteger;

public class Squfof {
	public static void main(String[] args) {
		Squfof squfof = new Squfof();
		long factor = squfof.squfof(new BigInteger("121197218027795137012962012581"));
		System.out.println(factor);
	}

	// https://en.wikipedia.org/wiki/Shanks%27_square_forms_factorization
	private long squfof(final BigInteger N) {
		// Initialize
		final int k = 1;
		long i = 0;
		final BigInteger kN = N.multiply(BigInteger.valueOf(k));

		long P0 = isqrt(kN).longValueExact(); // P_{i-1}
		long P1 = 0; // P_{i}
		final long sqrtKN = P0; // floor(sqrt(kN))
		long Q0 = 1; // Q_{i-1}
		long Q1 = BigInteger.valueOf(k).multiply(N)
				.subtract(BigInteger.valueOf(P0).pow(2)).longValueExact(); // Q_{i}
		long Q2; // Q_{i+1}
		do {
			long b = (sqrtKN + P0) / Q1; // b_{i}
			P1 = (b * Q1) - P0;
			Q2 = Q0 + b * (P0 - P1);
			// Slide window
			Q0 = Q1;
			Q1 = Q2;
			P0 = P1;
			if ((i++ % 2) == 0 && isPerfectSquare64(Q1))
				break;
		} while (true);

		// Q_i is a perfect square
		assert (BigInteger.valueOf(Q1).pow(2).compareTo(N) == 0);
		final long sqrtQ = isqrt(Q1);
		long b0 = (sqrtKN - P0) / sqrtQ;
		P0 = (b0 * sqrtQ) + P0;
		Q0 = sqrtQ;
		Q1 = kN.subtract(BigInteger.valueOf(P0).pow(2)).divide(BigInteger.valueOf(Q0)).longValueExact();
		do {
			long b = (sqrtKN + P0) / Q1; // b_{i}
			P1 = (b * Q1) - P0;
			Q2 = Q0 + b * (P0 - P1);
			if (P1 == P0)
				break;
			// Slide window
			Q0 = Q1;
			Q1 = Q2;
			P0 = P1;
		} while (true);

		return N.gcd(BigInteger.valueOf(P1)).longValueExact();
	}

	// http://goo.gl/IJ9q2K
	public boolean isPerfectSquare64(long x) {
		if (0xC840C04048404040L << x >= 0)
			return false;
		final int numberOfTrailingZeros = Long.numberOfTrailingZeros(x);
		if ((numberOfTrailingZeros & 1) != 0)
			return false;
		x >>= numberOfTrailingZeros;
		if ((x & 7) != 1 | x <= 0)
			return x == 0;
		final long tst = (long) Math.sqrt(x);
		return tst * tst == x;
	}

	public long isqrt(long in) {
		return isqrt(BigInteger.valueOf(in)).longValueExact();
	}

	// http://stackoverflow.com/users/1387163/eugene-fidelin
	public BigInteger isqrt(BigInteger in) {
		final BigInteger TWO = BigInteger.valueOf(2);
		int c;
		BigInteger n0 = TWO.pow(in.bitLength() / 2);
		BigInteger np = in;
		do {
			n0 = n0.add(in.divide(n0)).divide(TWO);
			c = np.compareTo(n0);
			np = n0;
		} while (c != 0);
		return n0;
	}
}
