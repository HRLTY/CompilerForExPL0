#include "tables.h"
#include "globalExtern.h"
#include "lexer.h"
#include <iostream>
#include <iomanip>
class Interpret {
public:
	order ib;//instruction buffer
	int stop,
		pc,
		tp, //top stack index
		bs;//base index
	Interpret(int ter):stop(ter)
	{ 
		st[1] = 0;//return addr
		st[2] = 0;//static link
		st[3] = -1; //dynamic link
		st[4] = btab[1].last;
		display[0] = display[1] = 0;
		tp = btab[2].vsize - 1;
		bs = 0;
		pc = tab[static_cast<int>(st[4])].adr;
	}
	void run()
	{
		while (pc < stop) {
			ib = code[pc++];
			if (execNextInstruc()) {
				break;
			}
		}
	}
	void errorInter(const char * str) {
		std::cout << "--->>error at code[" << pc - 1 << "] = "
			<< std::left << std::setw(4) << "f: " << std::left << std::setw(4) << (int)ib.f
			<< std::left << std::setw(4) << "x: " << std::left << std::setw(4) << ib.x
			<< std::left << "y: " << std::left << ib.y << " reason:" <<str<< "\n";
	}
	void increTop() { tp++; if (t > SMAX) errorInter("RUNTIME:stack full");}
	int execNextInstruc() //return 0 means success
	{
		switch (ib.f) {
		case 0:
			increTop();
			st[tp] = display[ib.x] + ib.y;
			break;
		case 1:
			increTop();
			st[tp] = st[(int)(display[ib.x] + ib.y)];
			break;
		case 2:
			increTop();
			st[tp] = st[static_cast<int>(st[(int)(display[ib.x] + ib.y)])];
			break;
		case 3:
		{int high = ib.y,
			low = ib.x,
			tem = bs;
		do {
			display[high--] = tem;
			tem = st[tem + 2];
		} while (high > low);
		break;
		}
		case 10:
			pc = ib.y;
			break;
		case 11:
			if (st[tp] == 0)//false then jump
				pc = ib.y;
			tp--;
			break;
		case 14:
		{int initial = st[tp - 1];
		if (initial <= st[tp])
			st[(int)st[tp - 2]] = initial;
		else {
			tp -= 3;
			pc = ib.y;
		}
		break;
		}
		case 15: {
			int adrOfInitial = st[tp - 2];
			int newInitial = st[adrOfInitial] + 1;
			if (newInitial <= st[tp]) {
				st[adrOfInitial] = newInitial;
				pc = ib.y;
			}
			else
				tp -= 3;
			break;
		}
		case 16:
		{
			int initial = st[tp - 1];
			if (initial >= st[tp])
				st[(int)st[tp - 2]] = initial;
			else {
				tp -= 3;
				pc = ib.y;
			}
			break;
		}
		case 17:
		{
			int adrOfInitial = st[tp - 2];
			int newInitial = st[adrOfInitial] - 1;
			if (newInitial >= st[tp]) {
				st[adrOfInitial] = newInitial;
				pc = ib.y;
			}
			else
				tp -= 3;
			break;
		}
		case 18: //Mark stack
		{
			int size = btab[tab[(int)ib.y].ref].vsize;
			if (tp + size > SMAX) {
				errorInter("RUNTIME:stack full");
				return 1;
			}
			tp += 5;
			st[tp - 1] = size - 1;
			st[tp] = ib.y; //new tab index
			break;
		}
		case 19: //call
		{
			int newBase = tp - ib.y;
			int newTabIndex = st[newBase + 4];
			int currentLev = tab[newTabIndex].lev;
			display[currentLev + 1] = newBase;
			int newTop = st[newBase + 3] + newBase; // vsize - 1 + newBase
			st[newBase + 1] = pc;//return addr
			st[newBase + 2] = display[currentLev]; //SL
			st[newBase + 3] = bs; //DL
			for (int j = tp + 1; j <= newTop; j++)
				st[j] = 0;
			bs = newBase;
			tp = newTop;
			pc = tab[newTabIndex].adr;
			break;
		}
		case 20://load array elem adr
		{
			int patab = ib.y;
			int index = st[tp];
			if (index < 0) {
				errorInter("RUNTIME:array index below 0 !");
				return 1;
			}
			if (atab[patab].bound <= index) {
				errorInter("RUNTIME:array subscript out of bound");
				return 1; //out of bound
			}
			tp--; //discard the index
			st[tp] += index;
			break;
		}
		case 24://load constant
			increTop();
			st[tp] = ib.y;
			break;
		case 27://read
		{
			switch((int)ib.y){
			case Symbol::unint: {
				int tem; std::cin >> tem;
				st[(int)st[tp]] = tem;
				break;
			}case Symbol::unreal: {
				float tem; std::cin >> tem;
				st[(int)st[tp]] = tem;
				break;
			}case Symbol::literal: {
				char ch; std::cin >> ch;
				st[(int)st[tp]] = ch;
				break;
			}
			default: break;
			}
			tp--;
			break;
		}
		case 28://write string
			//notice we didn't put string length on the stack.
			std::cout << stab[(int)ib.y];
			break;
		case 29://write basic
		{
			switch ((int)ib.y) {
			case Symbol::unint: {
				int tem = st[tp]; std::cout << tem;
				break;
			}case Symbol::unreal: {
				float tem = st[tp]; std::cout << tem;
				break;
			}case Symbol::literal: {
				char ch = st[tp]; std::cout << ch;
				break;
			}
			default: break;
			}
			tp--;
			break;
		}
		case 32: //exit proc
			tp = bs - 1;
			pc = st[bs + 1];
			bs = st[bs + 3]; //DL
			break;
		case 33: //from func
			tp = bs;
			pc = st[bs + 1];
			bs = st[bs + 3]; //DL
			break;
		case 34:
			st[tp] = st[(int)st[tp]];
			break;
		case 36:
			st[tp] = -st[tp];
			break;
		case 38: //infamous store
			st[(int)st[tp - 1]] = st[tp];
			tp -= 2;
			break;
		case 45:
			tp -= 1;
			st[tp] = st[tp] == st[tp + 1] ? 1 : 0;
			break;
		case 46:
			tp -= 1;
			st[tp] = st[tp] != st[tp + 1] ? 1 : 0;
			break;
		case 47:
			tp -= 1;
			st[tp] = st[tp] < st[tp + 1] ? 1 : 0;
			break;
		case 48:
			tp -= 1;
			st[tp] = st[tp] <= st[tp + 1] ? 1 : 0;
			break;
		case 49:
			tp -= 1;
			st[tp] = st[tp] > st[tp + 1] ? 1 : 0;
			break;
		case 50:
			tp -= 1;
			st[tp] = st[tp] >= st[tp + 1] ? 1 : 0;
			break;
		case 52:
			tp -= 1;
			st[tp] = static_cast<int>(st[tp] + st[tp + 1]);
			break;
		case 53:
			tp -= 1;
			st[tp] = static_cast<int>(st[tp] - st[tp + 1]);
			break;
		case 54:
			tp -= 1;
			st[tp] = (st[tp] + st[tp + 1]);
			break;
		case 55:
			tp -= 1;
			st[tp] = (st[tp] - st[tp + 1]);
			break;
		case 57:
			tp -= 1;
			st[tp] = static_cast<int>(st[tp] * st[tp + 1]);
			break;
		case 58:
			tp -= 1;
			if (st[tp + 1] == 0){
				errorInter("RUNTIME:divided by 0");
				return 1;
			}
			st[tp] = static_cast<int>(st[tp] / st[tp + 1]);
			break;
		case 60:
			tp -= 1;
			st[tp] = (st[tp] * st[tp + 1]);
			break;
		case 61:
			tp -= 1;
			if (st[tp + 1] == 0) {
				errorInter("RUNTIME:divided by 0");
				return 1;//divided by 0
			}
			st[tp] = (st[tp] / st[tp + 1]);
			break;
		default:
			errorInter("RUNTIME:instruction error or not implemented");
			return 1;
		}
		return 0;
	}
};