
/**
	Shell.cc

	@author Maurício Giordano
	@version 0.0.0.0.0.2 07/03/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <iomanip>
#include <stdexcept> 

#include "Shell.h"

#define BUFFER_SIZE 512

using namespace std;

Shell::Shell(string *dir)
{
	// saves the last statement typed by user
	this->lastStatement = new string();

	// default directory to save the database
	this->databaseDir = dir;

	// regex expressions
	this->RcreateDatabase = "[ \t\n]*create[ \t\n]+database[ \t\n]+([a-zA-Z][a-zA-Z0-9_-]*)[ \t\n]*;";
	this->RuseDatabase = "[ \t\n]*use[ \t\n]+([a-zA-Z][a-zA-Z0-9_-]*)[ \t\n]*;";
	this->RcreateTable = "[ \t\n]*create[ \t\n]+table[ \t\n]+([a-zA-Z][a-zA-Z0-9_-]*)[ \t\n]*\\([ \t\n]*(.*)[ \t\n]*\\)[ \t\n]*;";
	this->RcreateTableInside = "^[ \n\t]*([a-zA-Z][a-zA-Z0-9_]*)[ \n\t]+((char)[ \n\t]*\\[[ \n\t]*([0-9]+)[ \t\n]*\\]|(char)|(int)|(double)|(bool)|(blob)[ \n\t]*\\[[ \n\t]*([0-9]+)[ \t\n]*\\])[ \n\t]*$";
	this->RinsertInto = "[ \t\n]*insert[ \t\n]+into[ \t\n]+([a-zA-Z][a-zA-Z0-9_-]*)[ \t\n]*\\((.*)\\)[ \t\n]*values[ \t\n]*\\((.*)\\)[ \t\n]*;";
	this->RinsertIntoFields = "[ \t\n]*([a-zA-Z0-9._-]*)[ \t\n]*";
	this->RinsertIntoValues = "[ \t\n]*(\"([^\"]*)\"|'([^\"]*)'|([a-zA-Z0-9._-]*))[ \t\n]*";
	this->Rexit = "[ \t\n]*(exit)[ \t\n]*;";

	// authenticate the user and then begin
	this->authenticate();
}

Shell::~Shell()
{
	int i;

	// free/delete everything that has been allocated in heap
	if(this->lastStatement != NULL) delete this->lastStatement;

	for(i=0; i < this->databaseConnection.size(); ++i)
	{
		if(this->databaseConnection[i] != NULL) delete this->databaseConnection[i];
	}
}

void Shell::authenticate()
{
	string *typed = new string();

	cin >> *typed;

	if(typed->compare("root") != 0)
	{
	//  pretty output
		cout << "\033[1;31mErro:\033[1;30m Nome de usuario ou senha invalidos.\033[0m" << endl;

		// try again
		this->authenticate();
		return;
	}

	cin >> *typed;

	if(typed->compare("scc204") != 0)
	{
	//  pretty output
		cout << "\033[1;31mErro:\033[1;30m Nome de usuario ou senha invalidos.\033[0m" << endl;

		// try again
		this->authenticate();
		return;
	}

	delete typed;

	// begin the loop
	this->begin();
}

vector<string *> *Shell::executeRegex(char *statement, char *pattern)
{
	regex_t RE;	// Regex itself
	regmatch_t *REmatch = NULL; // Regex match
	int REgroups; // How many groups have been captured from the pattern

	// to return (will return all captured group except the first one)
	vector<string *> *captured = new vector<string *>();

	// check if the pattern is OK
	if(regcomp(&RE, pattern, REG_EXTENDED) != 0 )
	{
		fprintf(stderr, "Error compiling regular expression\n");
	}

	// capturing groups (quantity)
	REgroups = RE.re_nsub + 1;

	REmatch = (regmatch_t*) calloc(REgroups, sizeof(regmatch_t));

	// if has any match
	if(regexec(&RE, statement, REgroups, REmatch, 0) == 0)
	{
		char buffer[BUFFER_SIZE];

		for(int i = 1; i < REgroups; i++)
		{
			if(REmatch[i].rm_so != -1 && REmatch[i].rm_eo != -1)
			{
				// cut the needed part of the string
				// rm_so : start
				// rm_eo : end
				strncpy(buffer, statement + REmatch[i].rm_so, REmatch[i].rm_eo);

				// add '\0' at the end of the string
				buffer[REmatch[i].rm_eo - REmatch[i].rm_so] = '\0';

				// add the match to captured vector
				captured->push_back(new string(buffer));
			}
		}
	}

	if(REmatch != NULL) free(REmatch);

	return captured;
}

void Shell::freeRegex(vector<string *> *captured)
{
	// will free the captured vector
	if(captured != NULL)
	{
		for(int i = 0; i < captured->size() ; i++)
		{
			if(captured->at(i) != NULL) delete captured->at(i);
		}

		if(captured != NULL) delete captured;
	}
}

bool Shell::isCreateDatabase(string *returnDatabaseName)
{
	bool returnState = false;

	vector<string *> *captured = NULL;

	// typed statement
	char *statement = (char *) this->lastStatement->data();

	// captured groups
	captured = this->executeRegex(statement, (char *) this->RcreateDatabase);

	// if captured any
	if(captured->size() > 0)
	{
		// pos 0: database name (only group from RcreateDatabase pattern)
		returnDatabaseName->assign(captured->at(0)->c_str());

		// try to create database. If it already exists, return false
  		if(this->createDatabase(*returnDatabaseName, false))
  		{
  			returnState = true;
  		}
	}

	this->freeRegex(captured);

  	return returnState;
}

bool Shell::isUseDatabase(string *returnDatabaseName)
{
	bool returnState = false;

	vector<string *> *captured = NULL;

	// typed statement
	char *statement = (char *) this->lastStatement->data();

	// captured groups
	captured = this->executeRegex(statement, (char *) this->RuseDatabase);

	// if captured any
	if(captured->size() > 0)
	{
		// pos 0: database name (only group from RcreateDatabase pattern)
		returnDatabaseName->assign(captured->at(0)->c_str());

		// try to create database. If it already exists, return false
  		this->useDatabase(*returnDatabaseName);
  		
  		returnState = true;
	}

	this->freeRegex(captured);

  	return returnState;
}

bool Shell::isCreateTable(string *returnTableName)
{
	bool returnState = false;
	bool innerCheck = false;

	// will form the header to save in .dat
	ostringstream *tableHeader = new ostringstream();

	// will grab the fields from statement
	string *insideTestStatement = new string();

	// will split the stream (as it says)
	stringstream *splitStream = NULL;

	vector<string *> *captured = NULL;

	char *statement = (char *) this->lastStatement->data();

	captured = this->executeRegex(statement, (char *) this->RcreateTable);

	if(captured->size() > 0)
	{
		// from RcreateTable:
		// G0: table name
		// G1: inner statement
		returnTableName->assign(captured->at(0)->c_str());

		insideTestStatement->assign(captured->at(1)->c_str());

		splitStream = new stringstream(*insideTestStatement);

		// matched at least the outer statement
		// create table teste (blablabla);
		innerCheck = true;

		// split with comma delimiter 
		while(getline(*splitStream, *insideTestStatement, ','))
		{
			this->freeRegex(captured);

			// from RcreateTableInside
			// G0: field name
			// G1: field type
			// G2: field size (if not grabbed, size 1)
			captured = this->executeRegex((char *) insideTestStatement->c_str(), (char *) this->RcreateTableInside);

			// check for (ex. CPF char[12]) 
			if(captured->size() > 0)
			{
				// get field name
				*tableHeader << captured->at(0)->c_str() << ",";

				switch(captured->size())
				{
					// get field type and size 1
					case 3:
						*tableHeader << captured->at(2)->c_str() << ",";
						*tableHeader <<  "1";
						break;

					// get field type and size
					case 4:
						*tableHeader << captured->at(2)->c_str() << ",";
						*tableHeader << captured->at(3)->c_str();
						break;
				}

				// endl
				*tableHeader << endl;
			}
			else
			{
				innerCheck = false;
			}
		}

		if(innerCheck)
		{
			// will try to create table. If it already exists, return false
			if(this->currentDb->createTable(*returnTableName, tableHeader->str()))
			{
				returnState = true;
			}
		}
	}

	this->freeRegex(captured);

	delete insideTestStatement;
	delete splitStream;
	delete tableHeader;

  	return returnState;
}

bool Shell::isInsertInto(string *returnTableName, Node **returnNode)
{
	bool returnState = false; // final return
	bool problem = false;

	// for regex
	vector<string *> *captured = NULL;
	char *statement = (char *) this->lastStatement->data();

	int counter;

	// collected groups from regex match 
	string *tableName = new string();
	string *insideFields = new string();
	string *insideValues = new string();

	// simplify
	Table *currentTable = NULL;

	// to split the collected string
	stringstream *splitStream = NULL;

	int countFields = 0;
	int countValues = 0;
	int fieldPos;

	vector<string *> fields;
	vector<string *> values;

	map<string , int> fieldValue;

	Node *insertNode = NULL;

	captured = this->executeRegex(statement, (char *) this->RinsertInto);

	if(captured->size() > 0) // if matched
	{
		// from RinsertInto
		// G0: table name
		// G1: fields statement
		// G2: values statement
		tableName->assign(captured->at(0)->c_str()); // g0
		insideFields->assign(captured->at(1)->c_str()); // g1
		insideValues->assign(captured->at(2)->c_str()); // g2

		returnTableName->assign(captured->at(0)->c_str());

		// if table doesn't exists inside current db, do nothing
		if(this->currentDb->tableList[*tableName] == NULL)
		{
			returnState = false;
		}
		else
		{
			currentTable = this->currentDb->tableList[*tableName];

			splitStream = new stringstream(*insideFields);

			insertNode = currentTable->newNode(); // insert new node in curr db

			while(getline(*splitStream, *insideFields, ','))
			{
				this->freeRegex(captured);

				// from RinsertIntoFields
				// G0: field name
				captured = this->executeRegex((char *) insideFields->c_str(), (char *) this->RinsertIntoFields);

				if(captured->size() > 0) // if matched
				{
					fieldPos = currentTable->hasField(captured->at(0)); // check if field exists

					if(fieldPos != -1)
					{
						string *fieldToCheck = new string(captured->at(0)->c_str());

						fields.push_back(fieldToCheck); // add to array

						fieldValue[*fieldToCheck] = countFields;

						countFields ++;
					}
					else
					{
						problem = true; // if doesn't exists, break all
					}
				}

				if(problem) break;
			}
			
			delete splitStream;

			if(!problem)
			{
				splitStream = new stringstream(*insideValues);

				while(getline(*splitStream, *insideValues, ','))
				{
					// if has more values than fields, break
					if(countValues >= countFields)
					{
						problem = true;
						break;
					}

					this->freeRegex(captured);

					// from RinsertIntoValues
					// G0: full value (with ' and " if there is any)
					// G1: value without (' and ")
					captured = this->executeRegex((char *) insideValues->c_str(), (char *) this->RinsertIntoValues);

					if(captured->size() > 0) // if matched
					{
						// if not equal (ie. "Casa" and Casa, or 'A' and A)
						// did this to check if it is string
						if(*(captured->at(0)) != *(captured->at(1)))
						{
							// check if the field is a char field
							if(strcmp(currentTable->fieldType(fields.at(countValues)), "char") == 0)
							{
								// if given char
								if(captured->at(0)->c_str()[0] == '\'')
								{
									// if given char and has more than one character, problem!
									// or
									// if table represents a char[X] and it is given a char, problem!
									if(captured->at(1)->size() > 1
									|| currentTable->fieldSize(fields.at(countValues)) > 1)
									{
										problem = true;
									}
								}
								// if given char[X]
								else if(captured->at(0)->c_str()[0] == '"')
								{
									// if given char[X] is bigger than the field size, problem!
									if(captured->at(1)->size() > currentTable->fieldSize(fields.at(countValues)))
									{
										problem = true;
									}
								}
							}
							else
							{
								problem = true;
							}
						}
						else
						{
							// given char withou "" or ''
							if(strcmp(currentTable->fieldType(fields.at(countValues)), "char") == 0)
							{
								problem = true;
							}
						}

						// candidate to insert
						if(!problem)
						{
							string *valueToInsert = new string(captured->at(1)->c_str());

							values.push_back(valueToInsert);
						}

						countValues ++;
					}

					if(problem) break;
				}

				delete splitStream;

				// if fields equals values equals totalFields
				// obs. only limiting currentTable->countFields() because
				// for now we can't have default values to the collumns
				if(!problem
				&& countValues == countFields
				&& countFields == currentTable->countFields())
				{
					for(int i = 0; i < fields.size(); i++)
					{
						// set the field
						insertNode->setField(
							(char *) fields.at(i)->data(),
									 currentTable->fieldType(fields.at(i)),
									 values.at(i)
						);
					}

					// aux for
					string *fieldName = NULL;
					char *fieldType = NULL;
					int fieldSize, index;

					// recover the table header
					vector<tableStruct *> *tableHeader = currentTable->getTableHeader();

					// output the table name
					cout << *returnTableName << endl;

					// output the created node
					for(int i=0; i<tableHeader->size(); i++)
					{
						fieldName = new string();

						fieldName->assign(tableHeader->at(i)->name);

						fieldType = tableHeader->at(i)->type;

						fieldSize = tableHeader->at(i)->size;

						index = fieldValue[*fieldName];

						if(strcmp(fieldType, "char") == 0)
						{
							// std::setw(size): will output at least "size" characters
							// std::left: will fill the next content at left (default: right)
							// std::setfill(ch): will fill the remaining characters with "ch"
							cout << setw(fieldSize) << left << setfill(' ');
							cout << *(values.at(index));
						}
						else
						{
							cout << *(values.at(index));
						}

						if(i + 1 < tableHeader->size())
							cout << "|";

						delete fieldName;
					}

					cout << endl;

					returnState = true;

					*returnNode = insertNode; // will return for output

					currentTable->appendNode(insertNode); // will append in file
				}
				else
				{
					delete insertNode;

					problem = true;
				}
			}
		}
	}

	for(int i = 0; i < fields.size(); i++)
	{
		delete fields.at(i);
	}	

	for(int i = 0; i < values.size(); i++)
	{
		delete values.at(i);
	}

	this->freeRegex(captured);

	delete tableName;
	delete insideFields;
	delete insideValues;

	return returnState;
}

bool Shell::isExit()
{
	bool returnState = false; // final return

	//for regex
	vector<string *> *captured = NULL;
	char *statement = (char *) this->lastStatement->data();

	captured = this->executeRegex(statement, (char *) this->Rexit);

	if(captured->size() > 0) // if matched
	{
		returnState = true;
	}

	this->freeRegex(captured);

	return returnState;
}

void Shell::analyseStatement()
{
	bool hasMatched = false;
	bool matchedYet = false;

	string returnData;
	
	Node *insertNode;

	// check for [exit;] statement
	hasMatched = this->isExit();

	if(hasMatched)
	{
		return;
	}

	// check for [create database $1;] statement
	hasMatched = this->isCreateDatabase(&returnData);

	if(hasMatched)
	{
		matchedYet = true;
		cout << returnData << endl;
	}

	// check for [create database $1;] statement
	hasMatched = this->isUseDatabase(&returnData);

	if(hasMatched)
	{
		matchedYet = true;
		cout << returnData << endl;
	}

	// check for [create table $1 ($2);] statement
	if(!matchedYet)
		hasMatched = this->isCreateTable(&returnData);

	if(hasMatched && !matchedYet)
	{
		matchedYet = true;

		// output the table name
		cout << returnData << endl;

		// recover the table header
		vector<tableStruct *> *tableHeader = this->currentDb->tableList[returnData]->getTableHeader();

		// output the header
		for(int i=0; i<tableHeader->size(); i++)
		{
			cout << tableHeader->at(i)->name << ": ";
			cout << tableHeader->at(i)->type;

			if(tableHeader->at(i)->size > 1)
			{
				cout << "[" << tableHeader->at(i)->size << "]" << endl;
			}
			else
			{
				cout << endl;
			}
		}
	}

	// check for [insert into $1 ($2) values ($3);] statement
	if(!matchedYet)
		hasMatched = this->isInsertInto(&returnData, &insertNode);

	if(hasMatched && !matchedYet)
	{
		matchedYet = true;

		delete insertNode;
	}

	this->begin();
}

void Shell::begin()
{
	int counter = 1;
	char *statement = NULL; // typed statement

	char buff;

	while(true)
	{
		buff = fgetc(stdin);

		// realloc it
		statement = (char *) realloc(statement, (counter+1)*sizeof(char));
		statement[counter-1] = buff;
		statement[counter] = '\0';
		counter++;

		// until the user types ';'
		if(buff == ';')
		{
			break;
		}
	}

	// save the last typed statement
	this->lastStatement = new string(statement);

	// analyse it
	this->analyseStatement();
}

bool Shell::createDatabase(string name, bool useIt)
{
	int status;

	// stream to concatenate (more easely haha)
	ostringstream *os = new ostringstream();
	string *path = new string();

	// concatenate (eg. "./banco0")
	*os << *this->databaseDir << name;
	
	// get the String obj
	*path = os->str();

	delete os;

	// create the directory and save it status
	// if status equals -1, DB already exists (probably)
	status = mkdir(path->data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	// instantiate the new database
	Database *created = new Database(new string(name.data()), path);

	// save the connection
	// useless by now
	this->databaseConnection.push_back(created);

	// check if want to use it.
	// (it will be true only at the first time)
	if(useIt)
	{
		this->currentDb = created;
	}

	if(status == -1)
	{
		return false;
	}

	return true;
}

void Shell::useDatabase(string name)
{
	int status;

	// stream to concatenate (more easely haha)
	ostringstream *os = new ostringstream();
	string *path = new string();

	// concatenate (eg. "./banco0")
	*os << *this->databaseDir << name;
	
	// get the String obj
	*path = os->str();

	delete os;

	// create the directory and save it status
	// if status equals -1, DB already exists (probably)
	status = mkdir(path->data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	// instantiate the new database
	Database *created = new Database(new string(name.data()), path);

	// save the connection
	this->databaseConnection.push_back(created);

	this->currentDb = created;
}
