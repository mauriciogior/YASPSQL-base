
/**
	Database.h

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#ifndef __DATABASE_
#define __DATABASE_

#include <string>
#include <map>

#include "Table.h"
#include "Struct.h"

using namespace std;

class Database
{
private:
	/*
	 *	Will try to add a new table to database
	 *	Porpuse: when create a table or when use a database
	 *	It will instantiate all tables in heap
	 *
	 *	@param $1: table subpath (eg. Pessoa.dt)
	 */
	void addTable(string *);

public:
	string *name; // database name
	string *path; // database path

	map<string, Table *> tableList;
	
	/*
	 *	Will search database dir and instantiate (via addTable) all tables
	 *
	 *	@param $1: database name
	 *	@param $2: database path
	 */
	Database(string *, string *);
	~Database();

	/*
	 *	Will try to create a new table
	 *
	 *	@param $1: table name
	 *	@param $2: table header (comma separated)
	 *	@return success|fail
	 */
	bool createTable(string, string);
};

#endif
