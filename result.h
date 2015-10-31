#ifndef RESULT_H
#define RESULT_H

#include <string>
#include "query.h"
using namespace std;

struct Result {
    QueryType type;
    bool is_failed;
    string message;
};

struct ResultCreateTable : Result {
	ResultCreateTable() {
		type = CREATE_TABLE;
	}
};

struct ResultDropTable : Result {
	ResultDropTable() {
		type = DROP_TABLE;
	}
};

struct ResultCreateIndex : Result {
	ResultCreateIndex() {
		type = CREATE_INDEX;
	}
};

struct ResultDropIndex : Result {
	ResultDropIndex() {
		type = DROP_INDEX;
	}
};

struct ResultSelect : Result {
	vector<string> attribute_name;
    vector< vector<string> > record;
	ResultSelect() {
		type = SELECT;
	}
};

struct ResultInset : Result {
	ResultInset() {
		type = INSERT;
	}
};

struct ResultDelete : Result {
	ResultDelete() {
		type = DELETE;
	}
};
#endif // RESULT_H
