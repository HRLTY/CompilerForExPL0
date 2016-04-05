#include <iostream>
#include <istream>
#include <map>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

#ifndef __LEXER_H__
#define __LEXER_H__
#include "token.h"
/*class Num : public Token{
public:
  Num(int s, float v) :Token(s), value(v){}
  float value;
};
*/
/*class Word : public Token{
public:
  Word(int s, std::string str) : Token(s), lexeme(str){}
  std::string lexeme;
  Word(): Token(nul), lexeme(""){}
};
*/

class Lexer{
public:
  int lineCount = 0;
  int charCount = 0;
  std::istream* in;
  std::string currentLine;
  std::vector<std::string> source;
  char peek = ' ';
  std::map<std::string, Token> words;
  void reserve(Token w){ words.insert(std::pair<std::string, Token>(w.lexeme, w));}
  void error(std::string s) { std::cout << "line " << lineCount << " pos " << charCount << ' ' << s << '\n'; }
  Lexer(std::istream& i)
  {
    in = &i;
	//prepare for all kinds of keywords.
    reserve(Token(ifsym, "if"));
    reserve(Token(then, "then"));
    reserve(Token(elsesym, "else"));
    reserve(Token(dosym, "do"));
    reserve(Token(whilesym, "while"));
    reserve(Token(forsym, "for"));
    
    reserve(Token(proc, "procedure"));
    reserve(Token(func, "function"));
    
    reserve(Token(readsym, "read"));
    reserve(Token(writesym, "write"));
    
    reserve(Token(begin, "begin"));
    reserve(Token(end, "end"));
    
    reserve(Token(charsym,"char"));
    reserve(Token(realsym, "real"));
    reserve(Token(intsym, "integer"));
    reserve(Token(array, "array"));
	reserve(Token(of, "of"));
    reserve(Token(constsym, "const"));
    reserve(Token(varsym, "var"));
	reserve(Token(Symbol::to, "to"));
	reserve(Token(Symbol::downto, "downto"));
  }
  void printSource()
  {
	  if (source.size() >= 3)  std::cout << 'L' << lineCount - 2<<"->" << source[source.size() - 3]<< '\n';
	  if (source.size() >= 2)  std::cout << 'L' << lineCount - 1<<"->" << source[source.size() - 2] << '\n';
	  if (source.size() >= 1)  std::cout << 'L' << lineCount <<"->"<< source[source.size() - 1] << '\n';
	  for (size_t i = 0; i < charCount; i++)
	  {
		  std::cout << ' ';
	  }
	  std::cout << "^(approximately)\n";
  }
  void readch() 
  {
	  //peek = (char)in->get();
	  if (currentLine.empty()) {
		  if (in->eof()) {
			  peek = EOF;
			  return;
		  }
		  std::getline(*in, currentLine);
		  source.push_back(currentLine);
		  //peek = lineCount ? '\n' : ' ';
		  peek = '\n';
		  return;
	  }
	  peek = currentLine.front();
	  currentLine.erase(0, 1);
	  charCount++;
  }
  bool readch(char c){readch(); if(peek != c) return false; peek = ' '; return true;}
  Token scan()
  {
    for( ; ; readch()){
      if(peek == ' ' || peek == '\t') continue;
      else if(peek == '\n') {lineCount += 1; charCount = 0;}
      else if(peek == '\r' && readch('\n')){lineCount += 1; charCount = 0;}
      else if(peek == EOF) return Token(nul);
      else  break;
    }
	if (peek < 0) {
		int tem = peek;
		std::cout << "the char in the file is wrong.--->" << tem << "in line" << lineCount
			<< "pos" << charCount << '\n';
		return Token(nul);
	}
    switch(peek){
    case ':':
      if(readch('=')) return Token(assign,":="); else return Token(':');
    case '<':
      if(readch('=')) return Token(le, "<=");
      else if(peek == '>') {
        peek = ' '; return Token(ne, "<>");
      }else return Token('<');
    case '>':
      if(readch('=')) return Token(ge, ">="); else return Token('>');
    }
    if(isalpha(peek)){
      std::string s; s.push_back(peek);
      readch();
      while(isalnum(peek)){
        s.push_back(peek);
        readch();
      }
      auto search = words.find(s);
      if(search != words.end()){
        return search->second;
      }else
        return Token(id, s);
    }else if(isdigit(peek)){
      float t = peek - '0';
      readch();
      while(isdigit(peek)){
        t = 10 * t + peek - '0';
        readch();
      }
      if(peek == '.'){//real
        readch();
        float d = 0;
        int n = 1;
		if (!isdigit(peek))
			error("at least one digit expected after period'.'!");
        while(isdigit(peek)){
          d = 10 * d + peek - '0';
          n *= 10;
          readch();
        }
        return Token(Symbol::unreal, t + d / n);
      }else{ //integer
        return Token(Symbol::unint, t);
      }
    }else if(peek == '\''){//now we have a literal
      readch();
      if(!isalnum(peek)){
        error(std::string("literal expected!! found--> ") + peek);
        return Token(nul);
      }else{
        //char s[] = {peek, '\0'};
        Token t(literal, (float) peek);
        if(readch('\''))
          return t;
        else{
          error(std::string("literal terminator ''' expected! found --> ") + peek);
          return t;
        }
      }
    }else if(peek == '\"'){
      readch();
      std::string s;
      while(peek != '\"'){
		  if (peek <= 126 && peek >= 32) {
			  s.push_back(peek);
			  readch();
		  }
		  else {
			  error("string terminator expected before any char outside ascii 32-126!");
			  return Token(stringsym, s);
		  }
      }
      peek =' ';
      return Token(stringsym, s);
    }
    else{ //other char, simply return it as token's tag
      Token t(peek);
      peek = ' ';
      return t;
    } 
  }
  
};


#endif