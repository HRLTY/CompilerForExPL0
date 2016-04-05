// pl0.cpp : Defines the entry point for the console application.
//
#include "lexer.h"
#include "tables.h"
#include <iomanip>
#include <fstream>
#include "globalExtern.h"
std::string trans[] { "id", "charsym", "intsym", "realsym", "array", "of", "stringsym", "literal", //identifier, type and literals
    "ifsym", "then", "elsesym", "forsym", "whilesym", "dosym", //loop keywords
    "proc", "func", //procedure and function
    "begin", "end", //statement delimiter
    "readsym", "writesym",// io
    "constsym", "varsym", //declaration
    "assign", "ne", "ge", "le", "minus", "nul", //operators
	"unint", "unreal", //unsigned numbers
	"to", "downto" };


void emit(int fct) 
{
	if (lc >= CMAX)
		std::cout<<"code base full";
	code[lc].f = fct;
	lc += 1;
}

void emit1(int fct, float b)
{
	if (lc >= CMAX)
		std::cout << "code base full";
	code[lc].f = fct;
	code[lc].y = b;
	lc += 1;
}

void emit2(int fct, int a, int b)
{
	if (lc >= CMAX)
		std::cout << "code base full";
	code[lc].f = fct;
	code[lc].x = a;
	code[lc].y = b;
	lc += 1;
}
void printTablesForDebug()
{
	std::fstream output("code", std::ios_base::out);

	output << "CODE TABLE:\n" 
		<< std::left << std::setw(5) << "line"
		<< std::left << std::setw(4) << "f" 
		<< std::left << std::setw(4) << "x" 
		<< std::left << "y\n";
	for (int i = 0; i < lc; i++) {
		output
			<< std::left << std::setw(5) << i
			<< std::left << std::setw(4) << (int)code[i].f
			<< std::left << std::setw(4) << code[i].x
			<< std::left << code[i].y << "\n";
	}

}

/*
int main()// for Lexer homework only.
{
  //Lexer lexer(std::cin);
  std::fstream in("test", std::ios_base::in);
  std::cout <<(bool) in.is_open();
  Lexer lexer(in);
  Token t = lexer.scan();
  std::cout<<"line\tpos\tsymbol\tnum\tlexeme\n";
  while(t.tag != nul){
    std::cout<<lexer.lineCount<<'\t'<<lexer.charCount<<"\t";
    if(t.tag <= 126)
      std::cout<<std::left<<std::setw(10)<<char(t.tag);
    else
      std::cout<<std::left<<std::setw(10)<<trans[t.tag - 256];
    std::cout<<t.value<<"\t"<<t.lexeme<<'\n';
    t = lexer.scan();
  }
  return 0;
}*/



