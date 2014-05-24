
/**
	Shell.h

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#ifndef __SHELL_
#define __SHELL_

#include <string>
#include <vector>

#include "Database.h"
#include "Table.h"
#include "Node.h"
#include "Struct.h"

using namespace std;

class Shell
{
private:	
	string *lastStatement;
	string *databaseDir;

	// REGEX patterns
	const char *RcreateDatabase;
	const char *RuseDatabase;
	const char *RcreateTable;
	const char *RcreateTableInside;
	const char *RinsertInto;
	const char *RinsertIntoFields;
	const char *RinsertIntoValues;
	const char *Rexit;

	vector<Database *> databaseConnection;
	Database *currentDb;

public:
	/*
	 *	Shell constructor
	 *
	 *	@param $1: base directory
	 */
	Shell(string *);
	~Shell();

	/*
	 *	Will try to create a database
	 *
	 *	@param $1: database name
	 *	@param $2: define if will use it
	 *	@return success|fail
	 */
	bool createDatabase(string, bool);

	/*
	 *	Will try to use a database
	 *	(to implement)
	 *
	 *	@param $1: database name
	 */
	void useDatabase(string);

private:

	/*
	 *	Will try to authenticate the user
	 */
	void authenticate();

	/*
	 *	Will loop and grab every statement
	 */
	void begin();

	/*
	 *	Will define if statement matched any and output
	 */
	void analyseStatement();

	/*
	 *	Will check if is a "create database {$1};" kind
	 *
	 *	@param $1: database name to return
	 *	@return success|fail
	 */
	bool isCreateDatabase(string *);

	/*
	 *	Will check if is a "use {$1};" kind
	 *
	 *	@param $1: database name to return
	 *	@return success|fail
	 */
	bool isUseDatabase(string *);

	/*
	 *	Will check if is a "create table {$1} ({$2});" kind
	 *
	 *	@param $1: table name to return
	 *	@return success|fail
	 */
	bool isCreateTable(string *);

	/*
	 *	Will check if is a "insert into {$1} ({$2}) values ({$2});" kind
	 *
	 *	@param $1: table name to return
	 *	@param $2: new node return
	 *	@return success|fail
	 */
	bool isInsertInto(string *, Node **);

	/*
	 *	Will check if is a "exit;" kind
	 *
	 *	@return success|fail
	 */
	bool isExit();

	/*
	 *	Will try to match the statement to given pattern
	 *
	 *	@param $1: statement
	 *	@param $2: regex pattern
	 *	@return captured groups
	 */
	vector<string *> *executeRegex(char *, char *);

	/*
	 *	Will try to free a vector<string *> captured returned from executeRegex();
	 *
	 *	@param $1: captured groups
	 */
	void freeRegex(vector<string *> *);
};

#endif
