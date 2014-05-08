import java.util.LinkedList;

public class LRU<E> {

	private LinkedList<E> list = new LinkedList<E>();

	public void put(E o) {
		list.add(o);

	}

	public void toss() {
		list.removeFirst();

	}

	public boolean check(E o) {
		if (list.contains(o)) {
			return true;
		} else {
			return false;
		}
	}

	public int size() {
		return list.size();
	}

	public void donothing() {

	}

}
