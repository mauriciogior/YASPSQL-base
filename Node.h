
/**
	Node.h

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#ifndef __NODE_
#define __NODE_

#include <string>
#include <map>

#include "Table.h"
#include "Struct.h"

using namespace std;

class Table;

class Node
{
private:

	/*
	 *	Will set the field
	 *
	 *	@param field
	 *	@param value
	 *	@override value - int, bool, float, string *
	 */
	void setField(char *, int);
	void setField(char *, bool);
	void setField(char *, float);
	void setField(char *, string *);
	
public:

	/*
	 *	Will construct the Node
	 *
	 *	@param parent
	 *	@override parent - do nothing, do everything (basically)
	 */
	Node();
	Node(Table *);
	~Node();
	
	Table *parent;

	map<string, tableField *> fields;

	/*
	 *	Will return the node fields (its values)
	 *
	 *	@return map of values
	 */
	map<string, tableField *>* getFields();

	/*
	 *	Will return a field
	 *
	 *	@return void * to convert
	 */
	void *getField(char *);

	/*
	 *	Will set a field
	 *	Deduct if it is bool, char, int, etc and set it
	 *
	 *	@param field
	 *	@param type
	 *	@param value
	 */
	void setField(char *, char *, string *);
};

#endif
