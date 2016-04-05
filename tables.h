#include <string>
#ifndef __TABLES_H__
#define __TABLES_H__
const int MAX_IDENTIFIER_LEN = 30;
const int LMAX = 1000;
const int TMAX = 5000;
const int AMAX = 1000;
const int BMAX = 1000;
const int CMAX = 50000;
const int DMAX = 1000;
const int SMAX = 100000;
const int ARRAYBOUNDMAX = 100000;
const int STMAX = 10000;

struct tabEntry {
	std::string name;
	int link; //previous id location within the same subblock.
	int obj; //kind
	int typ; //type
	int ref; //ptr to atab or btab
	int lev;//static level for nested subprocedures.
	bool varParam;
	float adr; //location in stack for vars, code ptr for proc/func, value for all kinds of constant
};
struct order {
	unsigned char f;
	int x;
	float y;
};
union stackItem {
	int i;
	float d;
	char c;
};
struct btabEntry {
	int last; //ptr to last/current id location of the subblock in symbol table
	int lastpar; //ptr to the last parameter in symbol table
	int psize;// parameters and internal info size
	int vsize; //plus local varible size
};

struct atabEntry {
	int elety; //element type (char integer real)
	int bound; //high end of the array
	//element size always equals to one and total array size equals to bound
};

struct displayEntry {
	int level, ptr;
};
#endif