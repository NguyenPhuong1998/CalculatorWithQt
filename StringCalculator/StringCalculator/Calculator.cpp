#include "Calculator.h"

#include <math.h>
#include <QHashIterator>
//#include <QCharRef>

Calculator::Calculator()
{
	init();
}

void Calculator::init()
{
	_variables.insert("e", 2.718281828459045);
	_variables.insert("pi", 3.141592653589793);

	_natfunctions.insert("abs", fabs);
	_natfunctions.insert("acos", acos);
	_natfunctions.insert("asin", asin);
	_natfunctions.insert("atan", atan);
	_natfunctions.insert("cos", cos);
	_natfunctions.insert("exp", exp);
	_natfunctions.insert("floor", floor);
	_natfunctions.insert("ln", log);
	_natfunctions.insert("log", log10);
	_natfunctions.insert("sin", sin);
	_natfunctions.insert("sqrt", sqrt);
	_natfunctions.insert("tan", tan);
}

double Calculator::eval(const QString& e, const QHash<QString, double>& vars)
{
	Calculator calc(e);
	QHashIterator<QString, double> i(vars);
	while (i.hasNext()) {
		i.next();
		calc.set(i.key(), i.value());
	}

	return calc.compute();
}

inline int precedence(const QString& o)
{
	if (o == '+' || o == '-')
		return 1;
	else if (o == '*' || o == '/')
		return 2;
	else
		return 3;
}

// produce token list

void Calculator::parse(const QString& e)
{
	_input.clear();
	_output.clear();
	std::string _e = (e + ' ').toStdString();
	char* p = (char*)_e.c_str();
	QString name;
	TokenType type = NONE;
	while (char c = *p++)
	{
		switch (type) {
		case NONE:
			switch (c)
			{
			case '+': case '-': case '*': case '/': case '^':
				_input << c;
				break;
			case '(': case ')':
				_input << Token(SEP, QString(c));
				break;
			default:
				if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
					type = NUMBER;
					name = c;
				}
				else if (isalpha(c)) {
					type = NAME;
					name = c;
				}
				break;
			}
			break;
		case NUMBER:
			if (!((c >= '0' && c <= '9') || c == '.' || c == 'e' || (c == '-' && *(p - 2) == 'e'))) {
				_input << Token(name.toDouble());
				type = NONE;
				p--;
				break;
			}
			else
				name += c;
			break;
		case NAME:
			if (isalnum(c))
				name += c;
			else
			{
				_input << Token(NAME, name);
				p--;
				type = NONE;
			}
			break;
		default:
			break;
		}
	}

	// build reverse

	for (int i = 0; i < _input.size(); i++)
	{
		Token& token = _input[i];

		char op;
		switch (token.type)
		{
		case NUMBER: _output << token;
			break;
		case NAME:
			if (_input[i + 1].type == SEP && _input[i + 1].val == '(')
				_operators.push(token);
			else
				_output << token;
			break;
		case SEP:
			op = token.val[0].toLatin1();
			switch (op)
			{
			case '(': _operators.push(op);
				break;
			case ')':
				while (_operators.top().val != '(')
					_output << _operators.pop();
				_operators.pop();
				break;
			}
			break;

		case OP:
			op = token.val[0].toLatin1();
			if (op == '-' && (i == 0 || _input[i - 1].type == OP || _input[i - 1].val == '('))
			{
				_operators.push('_');
				break;
			}
			while (_operators.length() > 0 && _operators.top().val != '(' && (
				(op != '^' && precedence(token.val) <= precedence(_operators.top().val)) ||
				(op == '^' && precedence(token.val) < precedence(_operators.top().val))))
				_output << _operators.pop();
			_operators.push(token);
		default:
			break;
		}
	}

	while (_operators.length() > 0)
	{
		if (_operators.top().val == '(' || _operators.top().val == ')') {
			printf("Mismatch\n");
			return;
		}
		else
			_output << _operators.pop();
	}
}

double Calculator::value(const Calculator::Token& t)
{
	double def = 0.0;
	if (t.type == NAME)
		return _variables.value(t.val);
	else
		return (double)t.x;
}

double Calculator::compute()
{
	_operands.clear();

	// execute
	foreach(const Token & token, _output)
	{
		double x1, x2;
		switch (token.type)
		{
		case NUMBER:
			_operands.push(value(token));
			break;
		case OP:
			switch (token.val[0].toLatin1())
			{
			case '_':
				_operands.top() = -_operands.top();
				break;
			case '+':
				x2 = _operands.pop();
				x1 = _operands.top();
				_operands.top() = x1 + x2;
				break;
			case '-':
				x2 = _operands.pop();
				x1 = _operands.top();
				_operands.top() = x1 - x2;
				break;
			case '*':
				x2 = _operands.pop();
				x1 = _operands.top();
				_operands.top() = x1 * x2;
				break;
			case '/':
				x2 = _operands.pop();
				x1 = _operands.top();
				_operands.top() = x1 / x2;
				break;
			case '^':
				x2 = _operands.pop();
				x1 = _operands.top();
				_operands.top() = x1 + x2;
				if (x2 == 2)
					_operands.top() = x1 * x1;
				else
					_operands.top() = pow(x1, x2);
				break;
			}
			break;
		case NAME:
			if (const Function f = _natfunctions.value(token.val))
			{
				x1 = _operands.top();
				_operands.top() = f(x1);
			}
			/*else if (_functions.has(token.val))
			{
				x1 = _operands.pop();
				_operands.push(_functions(token.val, x1));
			}*/
			else
				_operands.push(value(token));
			break;
		default:
			break;
		}
	}
	double result = _operands.top();
	return result;
}
