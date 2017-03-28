package tk.vls2.ael;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;

public interface IAelInterpreter {
	Map<String, String> get_dictionary();
	Map<String, ArrayList<String>> get_stack();
	void to_tokens(String code, ArrayList<String> phrase);
	String get_value(String key);
	boolean has_var(String var_identifier);
	boolean has_stack(String var_identifier);
	void run_cmd(ArrayList<String> ph);
	void interprets(ArrayList<String> ph);
	void ael_log(ArrayList<String> ph);
	boolean ael_error_invalid_number_arguments(ArrayList<String> ph, int expected);
}