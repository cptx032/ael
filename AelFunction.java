package tk.vls2.ael;

import java.util.ArrayList;

public interface AelFunction {
	void execute(IAelInterpreter interpreter, ArrayList<String> phrase);
}