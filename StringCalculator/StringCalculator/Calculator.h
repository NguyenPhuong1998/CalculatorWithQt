#pragma once

#include <QString>
#include <QVector>
#include <QStack>
#include <QHash>

class Calculator
{
public:
	/**
	Creates a calculator
	*/
	Calculator();
	/**
	Creates a calculator and parses the given expression
	*/
	Calculator(const QString& e) { init(); parse(e); }
	/**
	Parses an expression to prepare for computation
	*/
	void parse(const QString& e);
	/**
	Computes the value of a previously parsed expression
	*/
	double compute();
	/**
	Parses and computes the value of an expression, optionally using a set of variable values
	*/
	static double eval(const QString& e, const QHash<QString, double>& vars = QHash<QString, double>());
	/**
	Sets the value of a variable for use in computation of an expression
	*/
	Calculator& set(const QString& var, double val) { _variables[var] = val; return *this; }

private:
	typedef double(*Function)(double);
	enum TokenType { NONE, OP, SEP, NAME, NUMBER };
	struct Token {
		double x = 0;
		QString val = "";
		TokenType type = NUMBER;
		Token() {}
		Token(char c) : type(OP), val(c) {}
		Token(TokenType t, const QString& v) : type(t), val(v) {}
		Token(double v) : type(NUMBER), x(v) {}
		Token(QString& v) : type(NAME), val(v) {}
	};
	QVector<Token> _input, _output;
	QStack<Token> _operators;
	QStack<double> _operands;
	QHash<QString, double> _variables;
	QHash<QString, Function> _natfunctions;
	void init();
	double value(const Token& t);
};

