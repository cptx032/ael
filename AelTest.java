import java.util.ArrayList;

public class AelTest {
	public static void main(String[] args) {
		AelInterpreter ael = new AelInterpreter();
		AelInterpreter.load_main_ael_functions(ael);

		ArrayList<String> phrase = new ArrayList<String>();
		if (args.length == 1) {
			String content = AelInterpreter.get_file_content(args[0]);
			ael.to_tokens(content, phrase);
			ael.interprets(phrase);
		}
	}
}