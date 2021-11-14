import java.util.*;
import java.math.BigInteger;
import java.util.Random;

public class Benchmarks {
	private Random r;

	public static void main(String[] args) {
		Benchmarks b = new Benchmarks();
		b.run();
	}

	public Benchmarks()
	{
		this.r = new Random();
	}

	public void run()
	{
		Random r = new Random();

		System.out.println(getPrime(50).multiply(getPrime(50))); // 30 siffror
		System.out.println(getPrime(33).multiply(getPrime(33))); // 20 siffror
		System.out.println(getPrime(17).multiply(getPrime(16))); // 10 siffror
		System.out.println(getPrime(9).multiply(getPrime(8))); // 5 siffror

		System.out.println(BigInteger.valueOf(6));

		// "många" siffror
		System.out.println(getPrime(9)
				.multiply(getPrime(9))
				.multiply(getPrime(9))
				.multiply(getPrime(9))
				.multiply(getPrime(9))
				.multiply(getPrime(9))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				.multiply(getPrime(8))
				);

		System.out.println(BigInteger.valueOf(6));
		
		makePair(10, 10); makePair(10, 20); makePair(10, 30); makePair(10, 40); makePair(10, 50); makePair(10, 60); makePair(10, 70);
		System.out.println(6);
		makePair(20, 10); makePair(20, 20); makePair(20, 30); makePair(20, 40); makePair(20, 50); makePair(20, 60); makePair(20, 70);
		System.out.println(6);
		makePair(30, 10); makePair(30, 20); makePair(30, 30); makePair(30, 40); makePair(30, 50); makePair(30, 60); makePair(30, 70);
		System.out.println(6);
		makePair(40, 10); makePair(40, 20); makePair(40, 30); makePair(40, 40); makePair(40, 50); makePair(40, 60); makePair(40, 70);
		System.out.println(6);
		makePair(50, 10); makePair(50, 20); makePair(50, 30); makePair(50, 40); makePair(50, 50); makePair(50, 60); makePair(50, 70);
		System.out.println(6);
		makePair(60, 10); makePair(60, 20); makePair(60, 30); makePair(60, 40); makePair(60, 50); makePair(60, 60); makePair(60, 70);
		System.out.println(6);
		makePair(70, 10); makePair(70, 20); makePair(70, 30); makePair(70, 40); makePair(70, 50); makePair(60, 70); makePair(70, 70);

		System.out.println(BigInteger.valueOf(6));

		System.out.println(getPrime(60).multiply(getPrime(60))); 
		System.out.println(getPrime(70).multiply(getPrime(70))); 

		System.out.println(0);
	}

	private BigInteger getPrime(int bitlength)
	{
		BigInteger b;
		int i = 0;
		do {
			b = new BigInteger(bitlength, r);
		} while (!b.isProbablePrime(40));

		return b;
	}

	private void makePair(int a, int b)
	{
		BigInteger x = getPrime(a);
		BigInteger y = getPrime(b);
		System.out.println(x.multiply(y));
	}
}
