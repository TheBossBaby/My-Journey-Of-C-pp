//This C++ impliments a calculator

 /*
	This program implements a basic expression calculator.
	Input from cin; output to cout.
	The grammar for input is:
	
	Statement:
		Expression
		Print
		Exit
		

	Print:
		;
	
	Exit:
		exit
	
	Sqrt:
		Sqrt "(" Expression ")"

	Power:
		pow( (Expression) , (Expression) )

	Expression
		Term
		Expression + Term
		Expression – Term
		Expression + Sqrt
		Expression - Sqrt

	Term:
		Primary
		Term * Primary
		Term / Primary
		Term % Primary

	Primary:
		Number
		( Expression )
		– Primary
		+ Primary
		Power
		Sqrt


	Number:
		floating-point-literal
	
	Input comes from cin through the Token_stream called ts.
*/

#include "std_lib_facilities.h"
#include "token.h"
//----------------------------------------------------------------------------------------------

const char let = '#'; //when user declare a variable 
const char Exit = 'E'; //used to exit calculator
const char print = ';'; //print value of expression
const char number = '8'; 
const char name = 'a'; //use to declare a 
const char now = 'n'; //re assign value to a variable
const char Sqrt = 'S'; //Square root of function
const char Pow = '^'; //Exponent of an expression
const char assignment = '=';

//----------------------------------------------------------------------------------------------
Token_stream ts; //define token stream
//Variable = name, value
struct Variable {
	string name;
	double value;
	Variable(string str, double val) :name(str), value(val) { }
};

vector<Variable> names;	//table of variable

//----------------------------------------------------------------------------------------------

double get_value(string s)
//takes value from a variable
{
	for (int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ",s);
}

//----------------------------------------------------------------------------------------------

void set_value(string s, double d)
//gives new value to a variable
{
	for (int i = 0; i<=names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ",s);
}

//----------------------------------------------------------------------------------------------

bool is_declared(string s)
//check if a string is defined previously or not
{
	for (int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}



double expression();

//----------------------------------------------------------------------------------------------

double Power()
//Handle pow() operation 
{
	double d{1};
	Token t = ts.get();
	int exponent{0};
	if(t.kind != '(')error("pow: expectet ( before base");

	if(t.kind == '('){
		d = expression();

	Token t2 = ts.get();
	if(t2.kind != ',') error("pow: expected \' , \' after expression");

	exponent = narrow_cast<int>(expression());

	Token t3 = ts.get();
	if(t3.kind != ')') error("pow: expected ) ");

	return pow(d, exponent);
	}
}

//----------------------------------------------------------------------------------------------

double Square_root()
//Handle sqrt(function) 
{
	double mid_result;
	Token B = ts.get();
	if(B.kind == '(')
		mid_result = expression();
	B = ts.get();
	if(B.kind != ')') error("sqrt: ')' expected");

	if(mid_result < 0) error("sqrt: Square root of negative number is not allowed");
	return sqrt(mid_result);
}

//----------------------------------------------------------------------------------------------

double primary()
//Handle number and ( ) and variable
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression(); //calculte the expression 
		t = ts.get(); //look for a colsing')' chracter
		if (t.kind != ')') error("')' expected");
		return d;
	}
	case '-':
		return - primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	case Pow:
		return Power();
	case Sqrt:
		return Square_root();
	default:
		error("primary expected");
	}
}

//----------------------------------------------------------------------------------------------

double term()
{
//handle *,/, %
	double left = primary();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		//Reviced by Pranav
		case '%':
		{
			double d = primary();
			if(d==0) error("divide by zero");
			left = fmod(left, d);
			break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

//----------------------------------------------------------------------------------------------

double expression()
{
//handle +,-
	double left = term();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

//----------------------------------------------------------------------------------------------

double declaration()
//Declare a vairable at run time
{
	Token t = ts.get();
	if (t.kind != name) error ("name expected in declaration"); //invalid declaration 
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of " ,name);
	double d = expression();
	names.push_back(Variable(name,d));
	return d;
}

//----------------------------------------------------------------------------------------------
void pre_define(string name, double val)
//Used to predefine varaible 
{
	names.push_back(Variable(name,val));
}
//----------------------------------------------------------------------------------------------

double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

//----------------------------------------------------------------------------------------------

void clean_up_mess()
//used in error handling 
//clear all the data till symbol print is not found
{
	ts.ignore(print);
}

//----------------------------------------------------------------------------------------------

const string prompt = "> "; //promt the user to input an expression
const string result = "= "; //gives the result

//----------------------------------------------------------------------------------------------

void calculate()
{
	while(true) 
	try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t=ts.get(); //take token from cin till print does not comes
		if (t.kind == Exit) return;
		ts.unget(t); //send t back to token stream 
		cout << result << statement() << endl;
	}
	catch(runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

//----------------------------------------------------------------------------------------------

int main()
{
	try {

		pre_define("k",1000);
		calculate();
		return 0;
	}
	catch (exception& e) {
		cerr << "exception: " << e.what() << endl;
		keep_window_open(";");
		return 1;
	}
	catch (...) {
		cerr << "exception\n";
		keep_window_open(";;");
		return 2;
	}
}
