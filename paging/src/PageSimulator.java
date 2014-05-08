import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Random;
import java.util.Scanner;

public class PageSimulator extends LRU<Integer> {

	public static void generateRandom(int num) {
		Random rand = new Random();

		FileWriter poorFile = null;
		try {
			poorFile = new FileWriter("poor");
			PrintWriter out = new PrintWriter(poorFile);

			for (int i = 0; i < num; i++) {
				out.println(rand.nextInt(99));
			}
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}
	
	public static void generateTypical(int num) {
		int bounds = num;
		Random rand1 = new Random();
		Random randtimes = new Random();
		FileWriter typicalFile = null;
		try {
			typicalFile = new FileWriter("typical");
			PrintWriter out = new PrintWriter(typicalFile);

			for (int i = 0; i < bounds; i++) {
				int a = rand1.nextInt(99);
				for (int j = 0; j < randtimes.nextInt(9) + 1; j++) {
					out.println(a);
				}

			}
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static void generateWell(int num) {
		Random rand2 = new Random();

		FileWriter wellFile = null;
		try {
			wellFile = new FileWriter("well");
			PrintWriter out = new PrintWriter(wellFile);

			for (int i = 0; i < num / 2; i++) {
				int a = rand2.nextInt(99);
				for (int j = 0; j < 10; j++) {
					out.println(a);
				}

			}
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}
	

	

	public static void main(String[] args) throws FileNotFoundException {
		int hitcount = 0;
		int hitsize = 0;
		Scanner pickone = new Scanner(System.in);
		Scanner queue = new Scanner(System.in);
		Scanner howmany = new Scanner(System.in);
		System.out.println("pick a string: well, poor, or typical");
		String decide = pickone.nextLine();
		System.out.println("How large should the pagequeue size get before we stop?");
		int queuesize = queue.nextInt();
		System.out.println("How many elements in the string should we generate or count?");
		int stringsize = howmany.nextInt();
		
//		generateRandom(stringsize);
//		generateTypical(stringsize);
//		generateWell(stringsize);

		LRU<Integer> FIFO = new LRU<Integer>();

		Scanner scanner = new Scanner(new File(decide));
	
		while (scanner.hasNextInt() && hitsize != stringsize) {

			int a = scanner.nextInt();
			if (FIFO.check(a) == false) {
				if (FIFO.size() == queuesize) {
					FIFO.toss();
					hitcount = hitcount + 1;
				}
				FIFO.put(a);
				hitsize++;
			} else if (FIFO.check(a) == true) {
				FIFO.donothing();
				hitsize++;
			}

		}

System.out.println("Page faults for " + stringsize + " elements from 0-99 in a " + queuesize + 
		" element page list using a " + decide + " behaved string: " + hitcount );
double percentage = ((double)hitcount / (double)stringsize * 100);
System.out.println("This is a  " + percentage + "% page fault hit rate.");
	}

}
