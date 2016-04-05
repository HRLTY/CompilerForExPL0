enum Symbol { id = 256, charsym, intsym, realsym, array, of, stringsym, literal, //identifier, type and literals //256-263
              ifsym, then, elsesym, forsym, whilesym, dosym, //loop keywords //264-269
              proc, func, //procedure and function 270-271
              begin, end, //statement delimiter 272-273
              readsym, writesym,// io 274-275
              constsym, varsym, //declaration 276-277
              assign, ne, ge, le, minus, nul,//operators 278-283
			  unint, unreal, //true numbers 284-285
			  to, downto}; //286-287
              
class Token{
public:
 Token(int t):tag(t),value(0.0),lexeme("") {}
 Token(int t, float v):tag(t),value(v),lexeme(""){}
 Token(int t, std::string s):tag(t),value(0.0),lexeme(s){}
  int tag;
  float value;
  std::string lexeme;
};
