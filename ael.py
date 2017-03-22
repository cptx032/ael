# coding: utf-8
# Author: Willie Lawrence
# Email cptx032 arroba gmail dot com
import sys
import random

AEL_DEFAULT_FUNCTION = '__ael_default_function'
AEL_VERSION = '1.0.0.2'
def is_a_blank_char(c):
	return c in (' ', '\t', '\n', '\r')

def get_file_content(file_path):
	file_ = open(file_path)
	content = file_.read()
	file_.close()
	return content

class aelinterpreter:
	def __init__(self, dictionary=dict(),
			functions=dict(), stdout=sys.stdout,
			stderr=sys.stderr, stdin=sys.stdin,
			stack=dict()):
		self.dictionary = dictionary
		self.functions = functions
		self.stack = stack

		self.stdout = stdout
		self.stderr = stderr
		self.stdin = stdin

	def error(self, msg):
		self.stderr.write(msg + '\n')

	def to_tokens(self, script, phrase):
		phrase.append('')
		block_counter = 0
		for i in range(len(script)):
			actual_char = script[i]
			if block_counter > 0:
				if actual_char == '{':
					block_counter += 1
					phrase[-1] += actual_char
				elif actual_char == '}':
					block_counter -= 1
					if block_counter != 0:
						phrase[-1] += actual_char
				else:
					phrase[-1] += actual_char
			else:
				if actual_char == '{':
					block_counter += 1
				elif is_a_blank_char(actual_char):
					if phrase[-1] != '':
						phrase.append('')
				elif actual_char == '.':
					phrase.append('.')
					phrase.append('')
				else:
					phrase[-1] += actual_char
		if phrase[-1] == '':
			phrase.pop(len(phrase)-1)

	def get_value(self, key):
		if key.startswith('{'):
			return key[1:-1]
		if key in self.dictionary:
			if self.dictionary[key].startswith('{'):
				return self.get_value(self.dictionary[key])
			return self.dictionary[key]
		return key

	def run_cmd(self, phrase):
		if phrase[0] in self.functions:
			self.functions[phrase[0]](self, phrase)
		else:
			new_phrase = list(phrase)
			new_phrase.insert(
				0,
				self.get_value(AEL_DEFAULT_FUNCTION)
			)
			self.functions[new_phrase[0]](self, new_phrase)

	def interprets(self, phrases):
		p = []
		for i in phrases:
			if i == '.':
				self.run_cmd(p)
				p = []
			else:
				p.append(i)

def arguments(length):
	def _decorator(function):
		def _final_function(ael, phrase):
			if len(phrase)-1 not in length:
				ael.error('Error: %s takes %s arguments (%s given)' % (phrase[0], length, len(phrase)-1))
			else:
				function(ael, phrase)
		return _final_function
	return _decorator

def _ael_trace(ael, phrase):
	for i in phrase[1:]:
		ael.stdout.write(ael.get_value(i) + ' ')
	ael.stdout.write('\n')

@arguments(length=[1,2])
def _ael_set(ael, phrase):
	if len(phrase) == 3:
		ael.dictionary[phrase[1]] = ael.get_value(phrase[2])
	elif len(phrase) == 2:
		ael.dictionary[phrase[1]] = ''

@arguments(length=[1])
def _ael_input(ael, phrase):
	ael.dictionary[phrase[1]] = ael.stdin.readline().replace('\n', '')

@arguments(length=[1])
def _ael_run(ael, phrase):
	content = ael.get_value(phrase[1])
	p = []
	ael.to_tokens(content, p)
	ael.interprets(p)

@arguments(length=[0, 1])
def _ael_exit(ael, phrase):
	if len(phrase) > 1:
		sys.exit(int(phrase[1]))
	sys.exit(0)

def _ael_nop(ael, phrase):
	pass

def _ael_print(ael, phrase):
	for i in phrase[1:-1]:
		ael.stdout.write(i)

def _ael_del(ael, phrase):
	if len(phrase) not in [0,1]:
		for i in phrase[1:]:
			del ael.dictionary[i]

def _ael_ls(ael, phrase):
	ael.stdout.write(str(ael.dictionary) + '\n')

def _ael_lf(ael, phrase):
	ael.stdout.write(str(ael.functions) + '\n')

@arguments(length=[2,3])
def _ael_add_number(ael, phrase):
	cast_func = int
	value1, value2 = ael.get_value(phrase[1]), ael.get_value(phrase[2])
	if '.' in value1 or '.' in value2:
		cast_func = float
	value1, value2 = cast_func(value1), cast_func(value2)
	if len(phrase) == 4:
		ael.dictionary[phrase[3]] = str(value1 + value2)
	else:
		ael.dictionary[phrase[1]] = str(value1 + value2)

@arguments(length=[2,3])
def _ael_mult_number(ael, phrase):
	cast_func = int
	value1, value2 = ael.get_value(phrase[1]), ael.get_value(phrase[2])
	if '.' in value1 or '.' in value2:
		cast_func = float
	value1, value2 = cast_func(value1), cast_func(value2)
	if len(phrase) == 4:
		ael.dictionary[phrase[3]] = str(value1 * value2)
	else:
		ael.dictionary[phrase[1]] = str(value1 * value2)

@arguments(length=[2,3])
def _ael_div_number(ael, phrase):
	cast_func = int
	value1, value2 = ael.get_value(phrase[1]), ael.get_value(phrase[2])
	if '.' in value1 or '.' in value2:
		cast_func = float
	value1, value2 = cast_func(value1), cast_func(value2)
	if len(phrase) == 4:
		ael.dictionary[phrase[3]] = str(value1 / value2)
	else:
		ael.dictionary[phrase[1]] = str(value1 / value2)

@arguments(length=[3])
def _ael_randint(ael, phrase):
	value1, value2 = int(ael.get_value(phrase[1])), int(ael.get_value(phrase[2]))
	ael.dictionary[phrase[3]] = str(random.randint(value1, value2))

@arguments(length=[2])
def _ael_loop(ael, phrase):
	loop = int(ael.get_value(phrase[1]))
	script = ael.get_value(phrase[2])
	p = []
	ael.to_tokens(script, p)
	while loop > 0:
		loop -= 1
		ael.interprets(p)

@arguments(length=[4, 6])
def _ael_if(ael, phrase):
	var1 = ael.get_value(phrase[1])
	op = ael.get_value(phrase[2])
	var2 = ael.get_value(phrase[3])

	pass_in_if = False
	if op == '==':
		pass_in_if = var1 == var2
	elif op == '!=':
		pass_in_if = var != var2
	elif op == '>':
		pass_in_if = float(var1) > float(var2)
	elif op == '>=':
		pass_in_if = float(var1) >= float(var2)
	elif op == '<':
		pass_in_if = float(var1) < float(var2)
	elif op == '<=':
		pass_in_if = float(var1) <= float(var2)

	if pass_in_if:
		p = []
		ael.to_tokens(ael.get_value(phrase[4]), p)
		ael.interprets(p)
	else:
		if len(phrase) == 7: # has else
			p = []
			ael.to_tokens(ael.get_value(phrase[6]), p)
			ael.interprets(p)

def _ael_log(phrase):
	for i in phrase:
		print '{%s}' % (i),
	print '\n'

def _ael_error_invalid_number_arguments(phrase, expected):
	if len(phrase)-1 != expected:
		print 'Error: ' + phrase[0] + ' takes exactly ' + str(expected) + ' arguments (' + str(len(phrase)-1) + ' given)'
		print 'Args:\n\t'
		_ael_log(phrase)
		return True
	return False

# stack push NAME VALUE.
# stack create NAME.
def _ael_stack(ael, phrase):
	command = phrase[1]
	if phrase[2] not in ael.stack:
		ael.stack[phrase[2]] = []
	if command == 'push':
		if _ael_error_invalid_number_arguments(phrase, 3):
			return
		ael.stack[phrase[2]].append(ael.get_value(phrase[3]))
	elif command == 'create':
		if _ael_error_invalid_number_arguments(phrase, 2):
			return
		if phrase[2] not in ael.stack:
			ael.stack[phrase[2]] = []
	elif command == 'pop':
		if _ael_error_invalid_number_arguments(phrase, 2):
			return
		ael.stack[phrase[2]].pop()
	elif command == 'destroy':
		if _ael_error_invalid_number_arguments(phrase, 2):
			return
		del ael.stack[phrase[2]]
	elif command == 'loop':
		if _ael_error_invalid_number_arguments(phrase, 3):
			return
		p = []
		ael.to_tokens(ael.get_value(phrase[3]), p)
		for i in range(len(ael.stack[phrase[2]])):
			ael.dictionary['STACK_VALUE'] = ael.stack[phrase[2]][i]
			ael.interprets(p)
	elif command == 'length':
		if _ael_error_invalid_number_arguments(phrase, 3):
			return
		ael.dictionary[phrase[3]] = len(ael.stack[phrase[2]])
	elif command == 'set':
		if _ael_error_invalid_number_arguments(phrase, 4):
			return
		index = int(ael.get_value(phrase[3]))
		if index < len(ael.stack[phrase[2]]):
			ael.stack[phrase[2]][index] = ael.get_value(phrase[4])
		else:
			print 'index out of bounds', 'fixme'
	elif command == 'get':
		if _ael_error_invalid_number_arguments(phrase, 4):
			return
		index = int(ael.get_value(phrase[3]))
		if index < len(ael.stack[phrase[2]]):
			ael.stack[phrase[4]] = ael.stack[phrase[2]][index]
		else:
			print 'index out of bounds', 'fixme' # FIXME
	elif command == 'erase':
		if _ael_error_invalid_number_arguments(phrase, 3):
			return
		index = int(ael.get_value(phrase[3]))
		if index < len(ael.stack[phrase[2]]):
			ael.stack[phrase[2]].pop(index)
		else:
			print 'index out of bounds', 'fixme'
	else:
		print 'ael stack: invalid command', phrase # FIXME

def _ael_strcat(ael, phrase):
	if len(phrase) != 3 and len(phrase) != 4:
		print 'strcat: wrong numbers of arguments' # FIXME
		return
	dest = phrase[3] if len(phrase) == 4 else phrase[1]
	ael.dictionary[dest] = ael.get_value(phrase[1]) + ael.get_value(phrase[2])

def load_main_ael_functions(ael):
	ael.functions['trace'] = _ael_trace
	ael.functions['set'] = _ael_set
	ael.functions['input'] = _ael_input
	ael.functions['run'] = _ael_run
	ael.functions['exit'] = _ael_exit
	ael.functions['del'] = _ael_del
	ael.functions['ls'] = _ael_ls
	ael.functions['lf'] = _ael_lf
	ael.functions['print'] = _ael_print
	# import
	ael.functions['#'] = _ael_nop
	ael.functions['+'] = _ael_add_number
	ael.functions['*'] = _ael_mult_number
	ael.functions['/'] = _ael_div_number
	ael.functions['loop'] = _ael_loop
	ael.functions['if'] = _ael_if
	ael.functions['stack'] = _ael_stack
	ael.functions['strcat'] = _ael_strcat

	ael.dictionary['__ael_version'] = AEL_VERSION

	ael.dictionary['\\n'] = '\n'
	ael.dictionary['\\t'] = '\t'
	ael.dictionary['\\b'] = '\b'
	ael.dictionary['\\r'] = '\r'
	ael.dictionary['\\a'] = '\a'
	ael.dictionary['__ael_default_function'] = 'run'

if __name__ == '__main__':
	ael = aelinterpreter()
	load_main_ael_functions(ael)
	phrase = []
	ael.to_tokens(get_file_content('main.ael'), phrase)
	ael.interprets(phrase)