import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Random;

public class StringGenerator {


	public void generateRandom(int num) {
		Random rand = new Random();

		
		FileWriter outFile = null;
		try {
			outFile = new FileWriter("numbers.txt");
			PrintWriter out = new PrintWriter(outFile);
			
			for (int i = 0; i < num; i++) {
				out.println(rand.nextInt(99));
			}
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}
}
