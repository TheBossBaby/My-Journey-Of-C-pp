//This C++ impliments a calculator

 /*
	This program implements a basic expression calculator.
	Input from cin; output to cout.
	The grammar for input is:
	
	Statement:
		Expression
		Print
		Quit
		

	Print:
		;
	
	Quit:
		exit
	
	Sqrt:
		Sqrt "(" Expression ")"

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
		Sqrt

	Number:
		floating-point-literal
	
	Input comes from cin through the Token_stream called ts.
*/

#include "std_lib_facilities.h"

//----------------------------------------------------------------------------------------------

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { } //Constructor for operators
	Token(char ch, double val) :kind(ch), value(val) { } //Constructor for token which are number
	Token(char ch, string n) :kind(ch), name(n) {} //Constructor for tokens like +, -, *, /
};

//----------------------------------------------------------------------------------------------
//stream of Token 

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { } //Empty token stream

	Token get(); 
	void unget(Token t) { buffer=t; full=true; } //inline function which makes token_stream full

	void ignore(char c);
};

//----------------------------------------------------------------------------------------------

const char let = 'L'; //when user declare a variable 
const char quit = 'Q'; //used to exit calculator
const char print = ';'; //print value of expression
const char number = '8'; 
const char name = 'a'; //use to declare a 
const char now = 'n'; //re assign value to a variable
const char Sqrt = '^'; //Square root of function

//----------------------------------------------------------------------------------------------

Token Token_stream::get()
{
	if (full) { full=false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
//Handels operators and send them in token stream
	case '(':	case ')':
	case '+':	case '-':
	case '*':	case '/':
	case '%':	case print:
	case '=':
		return Token(ch);
//Handels number and send them in token stream
	case '.':	case '0':
	case '1':	case '2':
	case '3':	case '4':
	case '5':	case '6':
	case '7':	case '8':
	case '9':
	{	cin.unget(); //send ch back to input stream
		double val;
		cin >> val;
		return Token(number,val);
	}
	default:
		if (isalpha(ch)) { //if ch is alphabet
			string s;
			s += ch; //send it to string 
//	Name:
//		Letter/Digit + Name
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch))) s+=ch;
			cin.unget(); //send characters back to cin
			if (s == "let") return Token(let); //user declraed  a variable	
			if (s == "exit") return Token(quit); //exit form calculator
			if(s == "Sqrt") return Token(Sqrt); //make a token to reprent sqare root operator 
			return Token(name,s);//token of kind token 
		}
		error("Bad token");
	}
}

//----------------------------------------------------------------------------------------------

void Token_stream::ignore(char c)
{
	if (full && c==buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin>>ch)
		if (ch==c) return;
}

//----------------------------------------------------------------------------------------------

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

Token_stream ts; //define token stream

double expression();

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
	//Make it primary part 
	case Sqrt:{
		//double mid_result = expression();
		double mid_result;
		Token B = ts.get();
		if(B.kind == '(')
			mid_result = expression();
		B = ts.get();
		if(B.kind != ')') error("sqrt: ')' expected");

		if(mid_result < 0) error("sqrt: Square root of negative number is not allowed");
		return sqrt(mid_result);
	}
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
		if (t.kind == quit) return;
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
