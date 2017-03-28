package tk.vls2.ael;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import tk.vls2.ael.*;

/***************** AEL FUNCTIONS ********************/
class AelTrace implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> phrase) {
		for (int i=1; i < phrase.size(); i++) {
			System.out.print(ael.get_value(phrase.get(i)) + " ");
		}
		System.out.println("");
	}
}

class AelSet implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if(ph.size() == 3) {
			ael.get_dictionary().put(ph.get(1), ael.get_value(ph.get(2)));
		}
		else if (ph.size() == 2) {
			ael.get_dictionary().put(ph.get(1), "");
		}
		else {
			System.err.println("Error: set takes 1 or 2 arguments (" + Integer.toString(ph.size()-1) + " given)");
			return;
		}
	}
}

class AelInput implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if(ph.size() != 2) {
			System.err.println("Error: input takes exactly 1 argument (" + Integer.toString(ph.size()-1) + " given)" );
			return;
		}
		Scanner scanner = new Scanner(System.in);
		ael.get_dictionary().put(ph.get(1), scanner.next());
	}
}

class AelRun implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if(ph.size()!=2) {
			System.err.println("Error: run takes exactly 1 argument (" + Integer.toString(ph.size()-1) + " given)");
			return;
		}
		String content = ael.get_value(ph.get(1));
		ArrayList<String> p = new ArrayList<String>();
		ael.to_tokens(content, p);
		ael.interprets(p);
	}
}

class AelExit implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		// DOES NOTHING
	}
}

class AelDel implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if(ph.size()==1) {
			System.err.println("Error: del takes 1 or more arguments (0 given)");
			return;
		}
		for(int i=1;i<ph.size();i++) {
			ael.get_dictionary().remove(ph.get(i));
		}
	}
}

class AelLS implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		// DOES NOTHING
	}
}

class AelLF implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		// DOES NOTHING
	}
}

class AelAddNumber implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size()!=3 && ph.size() != 4) {
			System.err.println("Error: '+' wrong number of arguments");
			return;
		}
		double value1 = Double.parseDouble(ael.get_value(ph.get(1)));
		double value2 = Double.parseDouble(ael.get_value(ph.get(2)));
		if (ph.size() == 4) {
			ael.get_dictionary().put(ph.get(3), Double.toString(value1 + value2));
		}
		else {
			ael.get_dictionary().put(ph.get(1), Double.toString(value1 + value2));
		}
	}
}

class AelMultNumber implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size()!=3 && ph.size() != 4) {
			System.err.println("Error: '+' wrong number of arguments");
			return;
		}
		double value1 = Double.parseDouble(ael.get_value(ph.get(1)));
		double value2 = Double.parseDouble(ael.get_value(ph.get(2)));
		if (ph.size() == 4) {
			ael.get_dictionary().put(ph.get(3), Double.toString(value1 * value2));
		}
		else {
			ael.get_dictionary().put(ph.get(1), Double.toString(value1 * value2));
		}
	}
}

class AelDivNumber implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size()!=3 && ph.size() != 4) {
			System.err.println("Error: '+' wrong number of arguments");
			return;
		}
		double value1 = Double.parseDouble(ael.get_value(ph.get(1)));
		double value2 = Double.parseDouble(ael.get_value(ph.get(2)));
		if (ph.size() == 4) {
			ael.get_dictionary().put(ph.get(3), Double.toString(value1 / value2));
		}
		else {
			ael.get_dictionary().put(ph.get(1), Double.toString(value1 / value2));
		}
	}
}

class AelImport implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		// FIXME
	}
}

class AelNOP implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		// NOP
	}
}

class AelLoop implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size() != 3) {
			System.err.println("Error: loop takes exactly 2 arguments (" + Integer.toString(ph.size()-1) + " given)" );
			return;
		}
		int loop = Integer.parseInt(ael.get_value(ph.get(1)));
		ArrayList<String> p = new ArrayList<String>();
		ael.to_tokens(ael.get_value(ph.get(2)), p);
		while (loop > 0) {
			loop -= 1;
			ael.interprets(p);
		}
	}
}

class AelIf implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size() != 5 && ph.size() != 7) {
			System.err.println("Error: 'if' : wrong number of arguments");
			return;
		}
		String var1 = ael.get_value(ph.get(1));
		String op = ph.get(2);
		String var2 = ael.get_value(ph.get(3));

		boolean pass_in_if = false;

		if (op.equals("==")) {
			pass_in_if = var1.equals(var2);
		}
		else if (op.equals(">")) {
			double v1 = Double.parseDouble(var1);
			double v2 = Double.parseDouble(var2);
			pass_in_if = v1 > v2;
		}
		else if (op.equals(">=")) {
			double v1 = Double.parseDouble(var1);
			double v2 = Double.parseDouble(var2);
			pass_in_if = v1 >= v2;
		}
		else if (op.equals("<")) {
			double v1 = Double.parseDouble(var1);
			double v2 = Double.parseDouble(var2);
			pass_in_if = v1 < v2;
		}
		else if (op.equals("<=")) {
			double v1 = Double.parseDouble(var1);
			double v2 = Double.parseDouble(var2);
			pass_in_if = v1 <= v2;
		}
		else {
			System.err.println("Error: if: Wrong operator " + op);
			return;
		}

		if (pass_in_if) {
			ArrayList<String> p = new ArrayList<String>();
			ael.to_tokens(ael.get_value(ph.get(4)), p);
			ael.interprets(p);
		}
		else {
			if (ph.size() == 7) {
				ArrayList<String> p = new ArrayList<String>();
				ael.to_tokens(ael.get_value(ph.get(6)), p);
				ael.interprets(p);
			}
		}
	}
}

class AelStack implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		String command = ph.get(1);

		if (!ael.get_stack().containsKey(ph.get(2))) {
			ael.get_stack().put(ph.get(2), new ArrayList<String>());
		}

		if (command.equals("push")) {
			if (ael.ael_error_invalid_number_arguments(ph, 3)) {
				return;
			}
			ael.get_stack().get(ph.get(2)).add(ael.get_value(ph.get(3)));
		}
		else if (command.equals("pop")) {
			if (ael.ael_error_invalid_number_arguments(ph, 2)) {
				return;
			}
			ael.get_stack().get(ph.get(2)).remove(ael.get_stack().get(ph.get(2)).size() -1);
		}
		else if (command.equals("destroy")) {
			if (ael.ael_error_invalid_number_arguments(ph, 2)) {
				return;
			}
			ael.get_stack().remove(ph.get(2));
		}
		else if (command.equals("loop")) {
			if (ael.ael_error_invalid_number_arguments(ph, 3)) {
				return;
			}
			ArrayList<String> p = new ArrayList<String>();
			ael.to_tokens(ael.get_value(ph.get(3)), p);
			for (int i=0; i < ael.get_stack().get(ph.get(2)).size(); i++) {
				ael.get_dictionary().put("STACK_VALUE", ael.get_stack().get(ph.get(2)).get(i));
				ael.interprets(p);
			}
		}
		else if (command.equals("length")) {
			if (ael.ael_error_invalid_number_arguments(ph, 3)) {
				return;
			}
			ael.get_dictionary().put(ph.get(3), Integer.toString(ael.get_stack().get(ph.get(2)).size()));
		}
		else if (command.equals("set")) {
			if (ael.ael_error_invalid_number_arguments(ph, 4)) {
				return;
			}
			int index = Integer.parseInt(ael.get_value(ph.get(3)));
			if (index < ael.get_stack().get(ph.get(2)).size()) {
				ael.get_stack().get(ph.get(2)).set(index, ael.get_value(ph.get(4)));
			}
			else {
				System.err.println("Error: stack set : index out of bounds");
			}
		}
		else if (command.equals("get")) {
			if (ael.ael_error_invalid_number_arguments(ph, 4)) {
				return;
			}
			int index = Integer.parseInt(ael.get_value(ph.get(3)));
			if (index < ael.get_stack().get(ph.get(2)).size()) {
				ael.get_dictionary().put(ph.get(4), ael.get_stack().get(ph.get(2)).get(index));
			}
			else {
				System.err.println("Error: stack get : index out of bounds");
			}
		}
		else if (command.equals("erase")) {
			if (ael.ael_error_invalid_number_arguments(ph, 3)) {
				return;
			}
			int index = Integer.parseInt(ael.get_value(ph.get(3)));
			if (index < ael.get_stack().get(ph.get(2)).size()) {
				ael.get_stack().get(ph.get(2)).remove(index);
			}
			else {
				System.err.println("Error: stack erase : index out of bounds");
			}
		}
		else {
			System.err.println("Error: stack : invalid command: " + command);
		}
	}
}

class AelStrCat implements AelFunction {
	public void execute(IAelInterpreter ael, ArrayList<String> ph) {
		if (ph.size() != 3 && ph.size() != 4) {
			System.err.println("Error: strcat : wrong numbers of arguments");
			return;
		}
		String destination = ph.size() == 4 ? ph.get(3) : ph.get(1);
		ael.get_dictionary().put(destination, ael.get_value(ph.get(1)) + ael.get_value(ph.get(2)));
	}
}

public class AelInterpreter implements IAelInterpreter {
	public static final String AEL_DEFAULT_FUNCTION = "__ael_default_function";
	public static final String AEL_VERSION = "1.0.0.2";

	public Map<String, String> dictionary = new HashMap<String, String>();
	public Map<String, ArrayList<String> > stack = new HashMap<String, ArrayList<String> >();
	public Map<String, AelFunction> functions = new HashMap<String, AelFunction>();

	public boolean ael_error_invalid_number_arguments(ArrayList<String> ph, int expected) {
		if (ph.size()-1 != expected) {
			System.out.println("Error: " + ph.get(0) + " takes exactly " + Integer.toString(expected) + " arguments (" + Integer.toString(ph.size()-1) + " given)" );
			System.out.print("Args:\n\t");
			this.ael_log(ph);
			return true;
		}
		return false;
	}

	public static String get_file_content(String file_path) {
		String result = "";
		try {
			BufferedReader br = new BufferedReader(new FileReader(file_path));
			String line;
			while ((line = br.readLine()) != null) {
				result += line;
			}
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		return result;
	}

	public void ael_log(ArrayList<String> ph) {
		for (int i=0; i < ph.size(); i++) {
			System.out.print("{" + ph.get(i) + "} ");
		}
		System.out.println("");
	}

	public static boolean is_a_blank_char(char _c) {
		return _c == ' ' || _c == '\t' || _c == '\n' || _c == '\r';
	}

	public static void load_main_ael_functions(AelInterpreter i) {
		i.functions.put("trace", new AelTrace());
		i.functions.put("set", new AelSet());
		i.functions.put("input", new AelInput());
		i.functions.put("run", new AelRun());
		i.functions.put("del", new AelDel());
		i.functions.put("ls", new AelLS());
		i.functions.put("lf", new AelLF());
		i.functions.put("+", new AelAddNumber());
		i.functions.put("*", new AelMultNumber());
		i.functions.put("/", new AelDivNumber());
		i.functions.put("import", new AelImport());
		i.functions.put("nop", new AelNOP());
		i.functions.put("loop", new AelLoop());
		i.functions.put("if", new AelIf());
		i.functions.put("stack", new AelStack());
		i.functions.put("strcat", new AelStrCat());

		i.get_dictionary().put("__ael_version", AelInterpreter.AEL_VERSION);
		i.get_dictionary().put(AelInterpreter.AEL_DEFAULT_FUNCTION, "run");
	}

	public Map<String, String> get_dictionary() {
		return this.dictionary;
	}

	public Map<String, ArrayList<String>> get_stack() {
		return this.stack;
	}

	public void to_tokens(String script, ArrayList<String> ph) {
		ph.add("");
		int block_counter = 0;
		char actual_char;
		for (int i=0; i < script.length(); i++) {
			actual_char = script.charAt(i);
			if (block_counter > 0) {
				if (actual_char == '{') {
					block_counter++;
					ph.set(ph.size()-1, ph.get(ph.size()-1) + actual_char);
				}
				else if (actual_char == '}') {
					block_counter--;
					if(block_counter != 0) {
						ph.set(ph.size()-1, ph.get(ph.size()-1) + actual_char);
					}
				}
				else {
					ph.set(ph.size()-1, ph.get(ph.size()-1) + actual_char);
				}
			}
			else {
				if (actual_char == '{') {
					block_counter++;
				}
				else if (AelInterpreter.is_a_blank_char(actual_char)) {
					if(ph.get(ph.size()-1) != "") {
						ph.add("");
					}
				}
				else if(actual_char == '.') {
					ph.add(".");
					ph.add("");
				}
				else {
					ph.set(ph.size()-1, ph.get(ph.size()-1) + actual_char);
				}
			}
		}

		if(ph.get(ph.size()-1) == "") {
			ph.remove(ph.size()-1);
		}
	}

	public String get_value(String key) {
		if(key.charAt(0) == '{') {
			return key.substring(1, key.length()-2);
		}

		if(this.has_var(key)) {
			if(this.dictionary.get(key).charAt(0) == '{') {
				return this.get_value(this.dictionary.get(key));
			}
			else {
				return this.dictionary.get(key);
			}
		}
		return key;
	}

	public boolean has_var(String var_identifier) {
		return this.dictionary.containsKey(var_identifier);
	}

	public boolean has_stack(String var_identifier) {
		return this.stack.containsKey(var_identifier);
	}

	public void run_cmd(ArrayList<String> ph) {
		if (this.functions.containsKey(ph.get(0))) {
			this.functions.get(ph.get(0)).execute(this, ph);
		}
		else {
			ArrayList<String> _new_phrase = new ArrayList<String>();
			_new_phrase.add(this.get_value(AEL_DEFAULT_FUNCTION));
			for (int i=0; i < ph.size(); i++) {
				_new_phrase.add(ph.get(i));
			}
			this.functions.get(_new_phrase.get(0)).execute(this, _new_phrase);
		}
	}

	public void interprets(ArrayList<String> ph) {
		ArrayList<String> p = new ArrayList<String>();
		for (int i=0; i < ph.size(); i++) {
			if (ph.get(i) == ".") {
				this.run_cmd(p);
				p.clear();
			}
			else {
				p.add(ph.get(i));
			}
		}
	}
}