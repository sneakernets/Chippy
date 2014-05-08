public class StringTester extends StringGenerator {

	@Override
	public void generateRandom(int num) {
		super.generateRandom(num);
	}

	public static void main(String[] args) {
		StringGenerator a = new StringGenerator();
		a.generateRandom(5000);

	}
}
