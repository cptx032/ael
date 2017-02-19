# coding: utf-8
# Author: Willie Lawrence
# Email cptx032 arroba gmail dot com
import sys

AEL_DEFAULT_FUNCTION = '__ael_default_function'
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
			stderr=sys.stderr, stdin=sys.stdin):
		self.dictionary = dictionary
		self.functions = functions

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

def _ael_pass(ael, phrase):
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

@arguments(length=[1])
def _ael_opr_unary_plus(ael, phrase):
	value = ael.get_value(phrase[1])
	if '.' in value:
		ael.dictionary[phrase[1]] = str(float(value)+1.0)
	else:
		ael.dictionary[phrase[1]] = str(int(value)+1)

def load_main_ael_functions(ael):
	ael.functions.update(
		trace=_ael_trace,
		set=_ael_set,
		input=_ael_input,
		run=_ael_run,
		exit=_ael_exit,
		ls=_ael_ls,
		lf=_ael_lf
	)
	ael.functions['#'] = _ael_pass
	ael.functions['print'] = _ael_print
	ael.functions['del'] = _ael_del
	ael.functions['++'] = _ael_opr_unary_plus
	ael.dictionary.update(
		__ael_default_function='run'
	)

if __name__ == '__main__':
	ael = aelinterpreter()
	load_main_ael_functions(ael)
	phrase = []
	ael.to_tokens(get_file_content('main.ael'), phrase)
	ael.interprets(phrase)