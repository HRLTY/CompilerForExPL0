#include "lexer.h"
#include "tables.h"
#include "parser.cpp"
#include "interpret.cpp"
#include <iostream>
#include <string>
#include <fstream>
//tables and globals
tabEntry tab[TMAX]; //symbol table
int t = 0; //table index
btabEntry btab[BMAX];
int b = 1;//procedure table index
atabEntry atab[AMAX];
int a = 0;//atab index
float st[SMAX];
int dtab[DMAX];
order code[CMAX]; //P-code base
int lc = 0; //current code location
int display[DMAX]; //for stack
std::string stab[STMAX]; //string table
int s = 0;//stab index

void printTablesForDebug();

int main(int argc, char** argv)
{
	//Lexer lexer(std::cin);
	std::cout << "please input file path:\n";
	char *file = new char[500];
	std::cin>>file;
	//file = R"(..\newtest4)";
	std::fstream in(file, std::ios_base::in);
	if (!in.is_open())
		std::cout << "\n----->FILE OPEN FAILED<-----\n";
	//std::cout << in.is_open();
	Lexer lexer(in);
	Parser parser(lexer);
	parser.program();
	printTablesForDebug();
	Interpret inter(lc);
	inter.run();
	return 0;
}

