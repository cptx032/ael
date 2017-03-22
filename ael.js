// heavy metal
var AEL_DEFAULT_FUNCTION = '__ael_default_function';
var AEL_VERSION = '1.0.0.2';
function is_a_blank_char(c) {
	if (c == ' ' || c == '\t' || c == '\r') {
		return true;
	}
	return false;
}

var aelinterpreter = function(){
	this.dictionary = {};
	this.functions = {};
	this.stack = {};
};
aelinterpreter.prototype.to_tokens = function(ael_code, phrase) {
	phrase.push('');
	let block_counter = 0;
	let actual_char;
	for (let i=0; i<ael_code.length; i++) {
		actual_char = ael_code[i];
		if (block_counter > 0) {
			if (actual_char == '{') {
				block_counter += 1;
				phrase[phrase.length-1] += actual_char;
			}
			else if (actual_char == '}') {
				block_counter -= 1;
				if (block_counter != 0) {
					phrase[phrase.length-1] += actual_char;
				}
			}
			else {
				phrase[phrase.length-1] += actual_char;
			}
		}
		else {
			if (actual_char == '{') {
				block_counter += 1;
			}
			else if (is_a_blank_char(actual_char)) {
				if (phrase[phrase.length-1] != '') {
					phrase.push('');
				}
			}
			else if (actual_char == '.') {
				phrase.push('.');
				phrase.push('');
			}
			else {
				phrase[phrase.length-1] += actual_char;
			}
		}
	}
	if (phrase[phrase.length-1] == '') {
		phrase.pop();
	}
};

aelinterpreter.prototype.get_value = function(key) {
	if (key[0] == '{') {
		return key.substr(1, key.length-2);
	}
	if (this.has_var(key)) {
		if (this.dictionary[key][0] == '{') {
			return this.get_value(this.dictionary[key]);
		}
		else {
			return this.dictionary[key];
		}
	}
	return key;
};

aelinterpreter.prototype.has_var = function(var_identifier) {
	return var_identifier in this.dictionary;
};

aelinterpreter.prototype.has_stack = function(var_identifier) {
	return var_identifier in this.stack;
};

aelinterpreter.prototype.run_cmd = function(phrase) {
	if (phrase[0] in this.functions) {
		this.functions[phrase[0]](this, phrase);
	}
	else {
		let _new_phrase = [];
		_new_phrase.push(this.get_value(AEL_DEFAULT_FUNCTION));
		for (let i=0; i < phrase.length; i++) {
			_new_phrase.push(phrase[i]);
		}
		this.functions[_new_phrase[0]](this, _new_phrase);
	}
};

aelinterpreter.prototype.interprets = function(phrase) {
	let p = [];
	for (let i=0; i<phrase.length; i++) {
		if (phrase[i] == '.') {
			this.run_cmd(p);
			p = [];
		}
		else {
			p.push(phrase[i]);
		}
	}
};

function _ael_trace(ael, phrase) {
	let s = '';
	for (let i=1; i<phrase.length; i++) {
		s += ael.get_value(phrase[i]) + ' ';
	}
	console.log(s);
}

function _ael_set(ael, phrase) {
	if (phrase.length == 3) {
		ael.dictionary[phrase[1]] = ael.get_value(phrase[2]);
	}
	else if (phrase.length == 2) {
		ael.dictionary[phrase[1]] = '';
	}
	else {
		console.error('Error: set takes 1 or 2 arguments');
	}
}

function _ael_run(ael, phrase) {
	if (phrase.length != 2) {
		console.error('Error: run takes excatly 1 argument');
		return;
	}
	let content = ael.get_value(phrase[1]);
	let p = [];
	ael.to_tokens(content, p);
	ael.interprets(p);
}

function _ael_exit(ael, phrase) {
	// does nothing because we are in a browser
	// we can redirects to a page
}

function _ael_del(ael, phrase) {
	if (phrase.length == 1) {
		console.error('Error: del takes 1 or more arguments (0 given)');
		return;
	}
	for (let i=0; i<phrase.length; i++) {
		delete ael.dictionary[phrase[i]];
	}
}

function _ael_ls(ael, phrase) {
	// TODO
}

function _ael_lf(ael, phrase) {
	// TODO
}

function _ael_nop(ael, phrase) {
	// does nothing
}

function _ael_add_number(ael, phrase) {
	if (phrase.length != 3 && phrase.length != 4) {
		console.error('Error: wrong numbers of arguments');
		return;
	}
	let value1 = parseFloat(ael.get_value(phrase[1]));
	let value2 = parseFloat(ael.get_value(phrase[2]));
	if (phrase.length == 4) {
		ael.dictionary[phrase[3]] = (value1 + value2).toString();
	}
	else {
		ael.dictionary[phrase[1]] = (value1 + value2).toString();
	}
}

function _ael_mult_number(ael, phrase) {
	if (phrase.length != 3 && phrase.length != 4) {
		console.error('Error: wrong numbers of arguments');
		return;
	}
	let value1 = parseFloat(ael.get_value(phrase[1]));
	let value2 = parseFloat(ael.get_value(phrase[2]));
	if (phrase.length == 4) {
		ael.dictionary[phrase[3]] = (value1 * value2).toString();
	}
	else {
		ael.dictionary[phrase[1]] = (value1 * value2).toString();
	}
}

function _ael_div_number(ael, phrase) {
	if (phrase.length != 3 && phrase.length != 4) {
		console.error('Error: wrong numbers of arguments');
		return;
	}
	let value1 = parseFloat(ael.get_value(phrase[1]));
	let value2 = parseFloat(ael.get_value(phrase[2]));
	if (phrase.length == 4) {
		ael.dictionary[phrase[3]] = (value1 / value2).toString();
	}
	else {
		ael.dictionary[phrase[1]] = (value1 / value2).toString();
	}
}

function _ael_loop(ael, phrase) {
	let loop = parseInt(ael.get_value(phrase[1]));
	let p = [];
	ael.to_tokens(ael.get_value(phrase[2]), p);
	while (loop > 0) {
		loop -= 1;
		ael.interprets(p);
	}
}

function _ael_if(ael, phrase) {
	if (phrase.length != 5 && phrase.length != 7) {
		console.error('ERROR in ael if');
		return;
	}
	let var1 = ael.get_value(phrase[1]);
	let op = ael.get_value(phrase[2]);
	let var2 = ael.get_value(phrase[3]);

	let pass_in_if = false;
	if (op == '==') {
		pass_in_if = var1 == var2;
	}
	else if (op == '!=') {
		pass_in_if = var1 != var2;
	}
	else if (op == '>') {
		pass_in_if = parseFloat(var1) > parseFloat(var2);
	}
	else if (op == '>=') {
		pass_in_if = parseFloat(var1) >= parseFloat(var2);
	}
	else if (op == '<') {
		pass_in_if = parseFloat(var1) < parseFloat(var2);
	}
	else if (op == '<=') {
		pass_in_if = parseFloat(var1) <= parseFloat(var2);
	}

	if (pass_in_if) {
		let p = [];
		ael.to_tokens(ael.get_value(phrase[4]), p);
		ael.interprets(p);
	}
	else {
		if (phrase.length == 7) { // has else
			let p = [];
			ael.to_tokens(ael.get_value(phrase[6]), p);
			ael.interprets(p);
		}
	}
}

function _ael_error_invalid_number_arguments(phrase, expected) {
	if (phrase.length-1 != expected) {
		console.error('Error: ' + phrase[0] + ' takes excatly ' + expected.toString() + ' arguments (' + (phrase.length-1).toString() + ' given )');
		return true;
	}
	return false;
}

function _ael_stack(ael, phrase) {
	let command = phrase[1];
	if (!(phrase[2] in ael.stack)) { // auto creation
		ael.stack[phrase[2]] = [];
	}
	if (command == 'push') {
		if (_ael_error_invalid_number_arguments(phrase, 3)) {
			return;
		}
		ael.stack[phrase[2]].push(ael.get_value(phrase[3]));
	}
	else if (command == 'pop') {
		if (_ael_error_invalid_number_arguments(phrase, 2)) {
			return;
		}
		ael.stack[phrase[2]].pop();
	}
	else if (command == 'destroy') {
		if (_ael_error_invalid_number_arguments(phrase, 2)) {
			return;
		}
		delete ael.stack[phrase[2]];
	}
	else if (command == 'loop') {
		if (_ael_error_invalid_number_arguments(phrase, 3)) {
			return;
		}
		let p = [];
		ael.to_tokens(ael.get_value(phrase[3]), p);
		for (let stack_i=0; stack_i < ael.stack[phrase[2]].length; stack_i++) {
			ael.dictionary['STACK_VALUE'] = ael.stack[phrase[2]][stack_i];
			ael.interprets(p);
		}
	}
	else if (command == 'length') {
		if (_ael_error_invalid_number_arguments(phrase, 3)) {
			return;
		}
		ael.dictionary[phrase[3]] = ael.stack[phrase[2]].length.toString();
	}
	else if (command == 'set') {
		if (_ael_error_invalid_number_arguments(phrase, 4)) {
			return;
		}
		let index = parseInt(ael.get_value(phrase[3]));
		if (index < ael.stack[phrase[2]].length) {
			ael.stack[phrase[2]][index] = ael.get_value(phrase[4]);
		}
		else {
			console.error('error: ael set: index out of stack bounds');
		}
	}
	else if (command == 'get') {
		if (_ael_error_invalid_number_arguments(phrase, 4)) {
			return;
		}
		let index = parseInt(ael.get_value(phrase[3]));
		if (index < ael.stack[phrase[2]].length) {
			ael.dictionary[phrase[4]] = ael.stack[phrase[2]][index];
		}
		else {
			console.error('error: ael get: index out of stack bounds');
		}
	}
	else if (command == 'erase') {
		if (_ael_error_invalid_number_arguments(phrase, 3)) {
			return;
		}
		let index = parseInt(ael.get_value(phrase[3]));
		if (index < ael.stack[phrase[2]].length) {
			// FIXME
		}
		else {
			console.error('error: ael erase: index out of stack bounds');
		}
	}
	else {
		console.error('ael stack: invalid command: ' + phrase[1]);
	}
}

function _ael_strcat(ael, phrase) {
	if (phrase.length != 3 && phrase.length != 4) {
		console.error('strcat: wrong numbers of arguments');
		return;
	}
	let dest;
	if (phrase.length == 4) {
		dest = phrase[3];
	}
	else {
		dest = phrase[1];
	}
	ael.dictionary[dest] = ael.get_value(phrase[1]) + ael.get_value(phrase[2]);
}

function lerp(a, b, x) {
	return a + ((b-a) * x);
}

function _ael_randint(ael, phrase) {
	if (phrase.length != 4) {
		console.error('ael randint error: wrong numbers of arguments', phrase);
		return;
	}
	let _min = parseInt(ael.get_value(phrase[1]));
	let _max = parseInt(ael.get_value(phrase[2]));
	ael.dictionary[phrase[3]] = lerp(_min, _max, Math.random());
}

function load_main_ael_functions(ael) {
	ael.functions['trace'] = _ael_trace;
	ael.functions["set"] = _ael_set;
	ael.functions['run'] = _ael_run;
	ael.functions['exit'] = _ael_exit;
	ael.functions['del'] = _ael_del;
	ael.functions['ls'] = _ael_ls;
	ael.functions['lf'] = _ael_lf;
	// ael.functions["print"] = _ael_print;
	// ael.functions["import"] = _ael_import;
	ael.functions['#'] = _ael_nop;
	ael.functions['+'] = _ael_add_number;
	ael.functions['*'] = _ael_mult_number;
	ael.functions['/'] = _ael_div_number;
	ael.functions['loop'] = _ael_loop;
	ael.functions['if'] = _ael_if;
	ael.functions['stack'] = _ael_stack;
	ael.functions['strcat'] = _ael_strcat;
	ael.functions['randint'] = _ael_randint;

	ael.dictionary['__ael_version'] = AEL_VERSION;

	ael.dictionary['\\n'] = '\n';
	ael.dictionary['\\t'] = '\t';
	ael.dictionary['\\b'] = '\b';
	ael.dictionary['\\r'] = '\r';
	ael.dictionary['\\a'] = '\a';
	ael.dictionary[AEL_DEFAULT_FUNCTION] = 'run';
}