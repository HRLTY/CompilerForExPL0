
#include "lexer.h"
#include "tables.h"
#include <string>
#include <iostream>
#include "globalExtern.h"

void emit(int fct);
void emit1(int fct, float b);
void emit2(int fct, int a, int b);

class Parser {
public:
	Lexer& lex;
	Token look;// = Token(Symbol::nul);
	int dx, level; //state for current block !!! 
	Parser(Lexer& l) : lex(l),look(Token(Symbol::nul)) { move(); };
	inline void move() { look = lex.scan(); }
	void error(std::string s) { std::cout << "line " << lex.lineCount << " pos " << lex.charCount << ' ' << s << '\n'; lex.printSource(); }
	inline bool match(int t) {return look.tag == t ? true : false;}
	inline void moveOnIfMatch(int t) { if (look.tag == t) move(); else expect(t); }//ignore currrent mismatch anyway
	inline bool matchThenMoveOn(int t) { if (look.tag == t) { move(); return true; } return false; }
	inline bool matchBasic() { return match(Symbol::charsym) || match(Symbol::intsym) || match(Symbol::realsym) 
								|| match(Symbol::unint) || match(Symbol::unreal) || match(Symbol::literal); }
	inline bool matchBasic(int t) { return t == Symbol::charsym || t == Symbol::intsym || t == Symbol::realsym
									|| t == Symbol::unint || t == Symbol::unreal || t == Symbol::literal; }
	int convertToNumType(int tp) {
		if (tp == Symbol::intsym)
			return Symbol::unint;
		else if (tp == Symbol::realsym)
			return Symbol::unreal;//unified tab plan.
		else if (tp == Symbol::charsym)
			return Symbol::literal;
		else return tp;
	}
	void expect(int t) {
		std::cout << "line " << lex.lineCount << " pos " << lex.charCount << ' ';
		std::cout << "Sytax error! expected ";
		if (t <= Symbol::id)
			std::cout << '\'' << (char)t << "'\n";
		else std::cout << '\'' << trans[t-Symbol::id] << "'\n";
		if (t != ';' && t != ':' && t != '.' && t != ',')
			move();//ignore current mismatch
		lex.printSource();
	}
	void program()
	{
		btab[1].last = t;
		btab[1].lastpar = 1;
		btab[1].psize = btab[1].vsize = 0;
		block(false, 1);
		moveOnIfMatch('.');
		std::cout << "Sytax parser finished!!!\n";
	}

	void enter(std::string lexeme, int kind)
	{
		if (t == TMAX)
			error("table full!!");
		int j, l;
		tab[0].name = lexeme;
		l = j = btab[dtab[level]].last;
		while (tab[j].name != lexeme) {
			j = tab[j].link;
		}
		if (j != 0) error("symbol " + lexeme + " redefined!");
		else {
			t += 1;
			tab[t].name = lexeme;
			tab[t].link = l;
			tab[t].obj = kind;
			tab[t].typ = Symbol::nul;
			tab[t].ref = 0;
			tab[t].lev = level;
			tab[t].adr = 0;
			tab[t].varParam = false; //inital value, which may be changed after by parameterList();
			btab[dtab[level]].last = t;
		}
	}
	int loc(std::string lexeme)
	{
		int i = level, j;
		tab[0].name = lexeme;
		do {
			j = btab[dtab[i]].last;
			while (tab[j].name != lexeme){
				j = tab[j].link;
			}
			i -= 1;
		} while (i >= 0 && j == 0);
		if (j == 0)
			error("id:" + lexeme + "not found!");
		return j;
	}
	Token constant()
	{
		Token t(Symbol::nul);
		bool sign = false;
		bool maybeLiteral = true;
		if (match('+') || match('-')) {
			sign = look.tag == '-' ? true : false;
			maybeLiteral = false;
			move();
		}
		if (match(Symbol::unint) || match(Symbol::unreal)) {
			//t.tag = match(Symbol::unint) ? Symbol::intsym : Symbol:: realsym;
			t.tag = look.tag; // unified since the table.
			t.value = sign ? -look.value : look.value;
			move();
		}else if (match(Symbol::literal)) {
			if (!maybeLiteral) 
				error("you have sign bit already, no char allowed!");
				//t.tag = Symbol::charsym;
			t.tag = look.tag;
			t.value = look.value;
			move();
		}else {
			error("constant expected!");
			//offer a unsigned int 0 instead
			t.tag = Symbol::unreal;
			t.value = 0;
			move();
		}
		return t;
	}
	void enterBlock()
	{
		if (b == BMAX)
			error("btab full!");
		b += 1;
		btab[b].last = 0;
		btab[b].lastpar = 0;
	}

	int enterArray(int type, int highBound)
	{
		if (highBound > ARRAYBOUNDMAX) {
			error("array bound too high");
			highBound = ARRAYBOUNDMAX;
		}
		if (a == AMAX)
			error("atab full!");
		if (highBound <= 0) {
			error("array bound less or equal 0");
			highBound = 1;
		}
		a += 1;
		atab[a].bound = highBound;
		atab[a].elety = type;
		return a;
	}
	int typeDec(int *size, int *ref)
	{
		if (matchBasic()) {
			*size = 1;
			*ref = 0;
			int tem = look.tag;
			move();
			return tem;
		}else if (match(Symbol::array)) {
			move();
			moveOnIfMatch('[');
			if (match(Symbol::unint)) {//we have a array bound
				*size = look.value;
				move();//eat the int
				moveOnIfMatch(']');
				moveOnIfMatch(Symbol::of);
				if (!matchBasic()) {
					error("array of basic type expected!");
					look.tag = Symbol::unint; //mock integer array elem instead.
				}
				*ref = enterArray(convertToNumType(look.tag), *size);
				move();
				return Symbol::array;
			}
			else { error("array bound error!"); }
		}
		else { error("type expected!"); }
		return Symbol::nul;
	}
	void paramiterList()
	{
		do {
			move(); //first time we know it's a '(', second time and so on we eat the ';'
			//handling one type of params
			bool varParam = false;
			if (match(Symbol::varsym)){//var param
				varParam = true;
				move();
			}
			int t0 = t;//one pos before params of this kind of type
			do {
				if (match(Symbol::id)){
					enter(look.lexeme, Symbol::varsym);
					move();//proceed to see is there are more of this type
				}else	expect(Symbol::id);
			} while (matchThenMoveOn(','));
			//mo more of this type. It's time to get the mystery type
			int t1 = t;
			moveOnIfMatch(':');
			int typ;
			if (!matchBasic()) {
				error("expect basic type for params.");
				look.tag = Symbol::unint;
			}
			typ = convertToNumType(look.tag);
			while (t0 < t1) {
				t0 += 1;
				tab[t0].typ = typ;
				tab[t0].ref = 0;
				tab[t0].adr = dx;
				tab[t0].lev = level;
				tab[t0].varParam = varParam; //KEY flag for var params.
				dx += 1;//afterall, basic type all have size one.
			}
			move();//proceed to potential ';'
		} while (match(';')); //another type continues
		moveOnIfMatch(')');
	}
	void block(bool isfun, int level)
	{
		int dx_bak = this->dx, level_bak = this->level;//remember to backup before you override and to restore before you go.

		this->dx = 5; // data allocation index
		this->level = level;
		int prt = t;// t-index
		if (level > LMAX)
			error("nested level too deep!");
		enterBlock();
		int prb = b;// b-index
		dtab[level] = b;
		tab[prt].typ = Symbol::nul;
		tab[prt].ref = prb;
		if (level >= 1 && match('(')){
			//move();
			paramiterList();
		}
		btab[prb].lastpar = t;
		btab[prb].psize = dx;
		if (isfun) {//handle return type
			moveOnIfMatch(':');
			if (!matchBasic()) {
				error("expect basic func return type");
				look.tag = Symbol::unint;
			}
			tab[prt].typ = convertToNumType(look.tag);	
			move();
		}
		if (level > 1 && match(';')) {
			move();
		}else if(level > 1)
			expect(';');
		if (match(Symbol::constsym))
			constDec();
		if (match(Symbol::varsym))
			variableDec();
		btab[prb].vsize = dx;
		while (match(Symbol::proc) || match(Symbol::func)) {
			bool isfunc = true;
			if (match(Symbol::proc))
				isfunc = false;
			move();
			procDec(isfunc);
			moveOnIfMatch(';');
		}
		tab[prt].adr = lc;
		compoundStatement();

		this->dx = dx_bak;
		this->level = level_bak;//remember to backup before you override and to restore before you go.
	}
	void compoundStatement()
	{
		moveOnIfMatch(Symbol::begin);
		do{
			statement();
		}while (matchThenMoveOn(';'));
		moveOnIfMatch(Symbol::end);
	}

	
	void constDef()
	{
		if (match(Symbol::id)) {//TODO: check redefine constant.
			/*if (loc(look.lexeme) != 0){
				error("redefined const " + look.lexeme);
			}*/
			enter(look.lexeme, Symbol::constsym);
			move();
			moveOnIfMatch('=');
			Token c = constant();
			tab[t].typ = c.tag;
			tab[t].ref = 0;
			tab[t].adr = c.value;
		}else
			expect(Symbol::id);
	}
	void constDec()
	{
		do {
			move();//(first time) I knew it's currently a const symbol. However, I leave its handle here because of the division of the grammar.
			constDef();
		} while (match(','));
		moveOnIfMatch(';');
	}
	void variableDec()
	{
		
			move();// I knew it's currently a var symbol. However, I leave its handle here because of the division of the grammar.
		do{
			varDef();
			moveOnIfMatch(';');
		} while (match(Symbol::id));
	}
	inline void varDef()
	{
		int t0 = t;
		if (match(Symbol::id)) {
			enter(look.lexeme, Symbol::varsym);//we pass kind not type
			move();
			while (match(','))
			{
				move(); if (match(Symbol::id)) {
					enter(look.lexeme, Symbol::varsym);
					move();
				}else {
					expect(Symbol::id);
				}
			}
			moveOnIfMatch(':');
			int t1 = t;
			int sz, ref; //size of the type and ptr to atab(possible)
			int tp = convertToNumType(typeDec(&sz, &ref));

			while (t0 < t1) {
				t0 += 1;
				tab[t0].typ = tp;
				tab[t0].ref = ref;
				tab[t0].lev = level;
				tab[t0].adr = dx;
				dx += sz; //we are calculating the storage for the block whenever we call it in the future. 
				//!!!IMPORTANT: We are not doing anything with stack. that's for call() to deal with.
			}
		}else {
			expect(Symbol::id);
		}
	}
	void procDec(bool isfun)
	{
		if (match(Symbol::id)) {
			if (isfun)
				enter(look.lexeme, Symbol::func);
			else
				enter(look.lexeme, Symbol::proc);
			move();//eat the id
			block(isfun, level+1);

			int f = isfun ? 33 : 32;
			emit(f);//#####DAMN I FORGOT TO RETURN !!!!!####
		}else {
			expect(Symbol::id);
		}
	}

	//-----------> handle all kinds of statement and P-code generation. <---------
	void statement()
	{
		int i;
		switch (look.tag) {
		case Symbol::begin:
			compoundStatement();
			break;
		case Symbol::ifsym:
			ifStatement();
			break;
		case Symbol::whilesym:
			whileStatement();
			break;
		case Symbol::forsym:
			forStatement();
			break;
		case Symbol::readsym:
			readStatement();
			break;
		case Symbol::writesym:
			writeStatement();
			break;
		case Symbol::id:
			i = loc(look.lexeme);
			if (i != 0) {
				switch (tab[i].obj)
				{
				case Symbol::varsym:
					assignment(i, tab[i].lev, tab[i].adr);
					break;
				case Symbol::proc:
					call(i);
					break;
				case Symbol::func:
					if (tab[i].ref == dtab[level])
						assignment(i, tab[i].lev + 1, 0);
					else
						error("function " + tab[i].name + " return value assignment not allowed here!");
					break;
				default:
					error("Wrong start id " + tab[i].name + " for a statement!");
					break;
				}
			}
			break;
		default:
			break;//empty statement
		}
	}
	inline bool matchRelation() 
	{	return match('=') || match(Symbol::ne) || match('<') 
		|| match(Symbol::le) || match('>') || match(Symbol::ge);}

	void relation() {
		int typ_fir = expression(), typ_sec, op;
		if (!matchRelation()){ 
			error("No comparision operator found.");
			op = Symbol::ne;//default not equal
		}
			op = look.tag;//one of relation operators
			move();
			typ_sec = expression();
			if (!(matchBasic(typ_fir) && matchBasic(typ_sec)))
				error("No non-basic type allowed for comparision.");
			if (typ_fir != typ_sec)//we may have to convert the two expr.
				error("warnning: inconsistant type in relation!");
			//since all float in stack, we do not convert for now.
		//two consistent basic type comparision
			switch (op) {
			case '=':
				emit(45); break;
			case Symbol::ne:
				emit(46); break;
			case '<':
				emit(47); break;
			case Symbol::le:
				emit(48); break;
			case '>':
				emit(49); break;
			case Symbol::ge:
				emit(50); break;
			default:
				break;
			}
		
	}
	int resultType(int t1, int t2)
	{
		if ((t1 == Symbol::unreal || t2 == Symbol::unreal) || (t1 == Symbol::realsym || t2 == Symbol::realsym))
			return Symbol::unreal;
		else if ((t1 == Symbol::unint || t2 == Symbol::unint) || (t1 == Symbol::intsym || t2 == Symbol::intsym))
			return Symbol::unint;
		else
			return Symbol::literal;
	}

	int expression() 
	{
		int op = Symbol::nul;
		if (match('+') || match('-')) {
			op = look.tag;
			move();
		}
		int typ_fir = term(), typ_sec;
		if (op == '-') emit(36);
		while (match('+') || match('-')) {
			op = look.tag;
			move();
			typ_sec = term();
			typ_fir = resultType(typ_fir, typ_sec);
			switch (typ_fir){
			case Symbol::literal:
			case Symbol::unint:
				if (op == '+') emit(52);
				else emit(53);
				break;
			case Symbol::unreal:
				if (op == '+') emit(54);
				else emit(55);
			default:
				break;
			}
		}
		return typ_fir;
	}

	int term()
	{
		int typ_fir = factor();
		while (match('*') || match('/')) {
			int op = look.tag;
			move();//eat the op!!
			int typ_sec = factor();
			typ_fir = resultType(typ_fir, typ_sec);
			switch (typ_fir) {
			case Symbol::literal:
			case Symbol::unint:
				if (op == '*') emit(57);
				else emit(58);
				break;
			case Symbol::unreal:
				if (op == '*') emit(60);
				else emit(61);
			default:
				break;
			}
		}
		return typ_fir;
	}
	int selector(tabEntry t)
	{
		move();//eat the '['
		int typ = expression();
		if (t.typ != Symbol::array) //not true array, better stop before subaddress it.
			error("not a array when trying to subaddress!");
		else{
			if (!(typ == Symbol::literal || typ == Symbol::unint))
				error("subindex type not unsighed int!");
			emit1(20, t.ref);
			moveOnIfMatch(']');
			return atab[t.ref].elety;
		}
		return Symbol::nul;
	}
	int factor()
	{
		int i;
		int tem;
		switch (look.tag) {
		case Symbol::unint:
		case Symbol::unreal:
			emit1(24, look.value);
			tem = look.tag;
			move();
			return tem;
		case '(':
			move();
			tem = expression();
			moveOnIfMatch(')');
			return tem;
		case Symbol::id:
			i = loc(look.lexeme);
			if (i == 0) error("undefined id for factor!");
			
			switch (tab[i].obj) {
			case Symbol::func:
				call(i);
				return tab[i].typ;
			case Symbol::constsym:
				emit1(24, tab[i].adr);//load the constant value
				move();
				return tab[i].typ;
			case Symbol::varsym:
				move();
				if (match('[')) {//we got an array element
					int f = tab[i].varParam ? 1 : 0; //load value or address
					emit2(f, tab[i].lev, tab[i].adr);
					int eleTyp = selector(tab[i]);
					if (matchBasic(eleTyp)) //any intsym or unint issue??
						emit(34);
					return eleTyp;
				}else { // a simple variable
					int f = tab[i].varParam ? 2 : 1; //load value indirectly or directly.
					emit2(f, tab[i].lev, tab[i].adr);
					return tab[i].typ;
				}
			default:
				error("wrong id:"+ look.lexeme + " for factor!");
			}
			break;
		default:
			std::string tem = look.tag >= 256 ? trans[look.tag - 256] : std::string(1, look.tag);
			error("->" + tem + "<- is a bad factor!!!");
		}
		return Symbol::nul;
	}

	void assignment(int i, int lev, int adr)
	{
		int f = tab[i].varParam ? 1 : 0; // load value or address to stack top
		emit2(f, lev, adr); //this is absolutely a Lvalue!!! must use address of the variable.
		int typ_fir = tab[i].typ;
		move();//eat the id
		if (match('['))
			typ_fir = selector(tab[i]);
		moveOnIfMatch(Symbol::assign);
		int typ_sec = expression();
		if (typ_fir == typ_sec) {
			if (matchBasic(typ_fir))//identical basic assignment
				emit(38);
			else
				error("non basic type assignment encountered!");
		}
		else if (typ_fir == Symbol::unreal && matchBasic(typ_sec)) { //cast rvalue(any basic) to real lvalue.
			emit(38);
		}else if(typ_fir == Symbol::unint && typ_sec == Symbol::literal){//TODO: maybe constant is allowed??
			emit(38);
		}else {
			// NO We don't do other type cast.
			error("assignment type conflict!");
			emit(38);//suppose we do
		}
	}

	void ifStatement()
	{
		move();//eat ifsym
		relation();// now a bool in the stack top
		int lc1 = lc;
		emit(11); //jumpc, we'll come back for the y later
		moveOnIfMatch(Symbol::then);
		statement();
		if (matchThenMoveOn(Symbol::elsesym)) {
			int lc2 = lc;
			emit(10); //immediate jump to y
			code[lc1].y = lc;
			statement();
			code[lc2].y = lc;
		}else
			code[lc1].y = lc;
	}

	void whileStatement()
	{
		move(); //eat while
		int lc1 = lc;
		relation();
		int lc2 = lc;
		emit(11); //false then jump to y
		moveOnIfMatch(Symbol::dosym);
		statement();
		emit1(10, lc1);//unconditionally jump back to test condition
		code[lc2].y = lc; //false point
	}

	void forStatement()
	{
		move();//eat the for
		int typ_fir, typ_sec;
		if (match(Symbol::id)) {
			int i = loc(look.lexeme);
			if (!i){
				typ_fir = Symbol::unint;
			}else if (tab[i].obj == Symbol::varsym) {//variable is the only kind allowed
				typ_fir = tab[i].typ;
				if (tab[i].varParam)
					error("should be a variable.");
				else
					emit2(0, tab[i].lev, tab[i].adr);
			}else {
				typ_fir = Symbol::unint;
				error("should be a variable.");
			}
			move();
		}else {
			expect(Symbol::id);
		}
		moveOnIfMatch(Symbol::assign);
		typ_sec = expression();
		if (typ_fir != typ_sec)
			error("type inconsistency");
		int f = 14;
		if (match(Symbol::to))
			move();
		else if (match(Symbol::downto)) {
			move();
			f = 16;
		}else
			expect(Symbol::to);
		int typ_thr = expression();
		if (typ_fir != typ_thr)
			error("type inconsistency");
		int lc1 = lc;
		emit(f);
		moveOnIfMatch(Symbol::dosym);
		int lc2 = lc;
		statement();
		emit1(f + 1, lc2);
		code[lc1].y = lc;
	}
	bool assignable(int lvalue, int rvalue)
	{
		if (!(matchBasic(lvalue) && matchBasic(rvalue))) {
			error("we need basic type for param");
			return false;
		}
		if (lvalue != rvalue) {
			if (lvalue == Symbol::unreal)
				return true;
			else if (lvalue == Symbol::unint && rvalue == Symbol::literal)
				return true;
			else {
				error("type dismatch!!");
				return false;
			}
		}
		return true;
	}
	void call(int i)
	{
		emit1(18, (float) i); //mark stack
		int lastp = btab[tab[i].ref].lastpar;
		int cp = i;
		move();//eat the identifier.
		if (matchThenMoveOn('(')) {//actual parameter list exist
			do {
				if (cp >= lastp) //no params needed according to the declaration
					error("Too many param.");
				else {
					cp += 1;
					if (tab[cp].varParam) {//pass by reference/address
						if (match(Symbol::id)) {
							int k = loc(look.lexeme);
							move();
							if (k) {
								if (tab[k].obj != Symbol::varsym)
									error("we need a variable for var param...");
								if (!match('[')){//not typ array then we check type consistency
									int lvalue = tab[cp].typ, rvalue = tab[k].typ;
									assignable(lvalue, rvalue);
								}
								if (tab[k].varParam)//he himself is a var param from caller.
									emit2(1, tab[k].lev, tab[k].adr);
								else
									emit2(0, tab[k].lev, tab[k].adr);
								if (match('[')) { //we got a array element
									int typ = selector(tab[k]);
									assignable(tab[cp].typ, typ);
									//if (typ != tab[cp].typ)
									//	error("inconsistent param type !");
								}
							}
							else
								error("undefined id for param.");
						}
						else {
							error("we need a variable for var param...");
						}
					}
					else { //pass by value
						int typ = expression();
						assignable(tab[cp].typ, typ);
						//if (typ == tab[cp].typ) ; //no need to copy array or access record.
						//else if ((typ == Symbol::unint || typ == Symbol::literal) && tab[cp].typ == Symbol::unreal) {
						//	;//we don't need convertion since our stack use float entry..
						//}else {
						//	error("inconsistent param type !");
						//}
					}
				}
			} while (matchThenMoveOn(',')); //next param
			moveOnIfMatch(')');
		} 
		//param list handling finished/unencountered.
		if (cp < lastp)
			error("too few actual params.");
		emit1(19, (float) btab[tab[i].ref].psize - 1);
		if (tab[i].lev < level)
			emit2(3, tab[i].lev, level);
	}

	//TODO read and write 
	void readStatement()
	{
		move();
		moveOnIfMatch('(');
		do {
			if (match(Symbol::id)) {
				int i = loc(look.lexeme);
				move();
				if (i != 0) {
					if (tab[i].obj == Symbol::varsym){
						int f = tab[i].varParam ? 1 : 0;
						emit2(f, tab[i].lev, tab[i].adr);
						int typ = tab[i].typ;
						if (matchThenMoveOn('[')) {
							typ = selector(tab[i]);
						}
						if (matchBasic(typ))
							emit1(27, typ);
						else error("not basic type when reading");
					}else error("read must be a var");
				}else error("undefined var");
			}else expect(Symbol::id);
		} while (matchThenMoveOn(','));
		moveOnIfMatch(')');
	}

	void writeStatement()
	{
		bool exprExpect = true;
		move();
		if (matchThenMoveOn('(')) {
			if (match(Symbol::stringsym)) {
				exprExpect = false;
				stab[s] = look.lexeme;
				emit1(28, s++);//write string.
				move();
				if (matchThenMoveOn(')'))
					return;
				else if (matchThenMoveOn(','))
					exprExpect = true;
			}
			if (exprExpect) {
				int typ = expression();
				if (matchBasic(typ))
					emit1(29, typ);
				else error("expect basic type when writing.");
			}
		}else expect('(');
		moveOnIfMatch(')');
	}
};