
/**
	Table.h

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#ifndef __TABLE_
#define __TABLE_

#include <vector>

#include "Node.h"
#include "Struct.h"

using namespace std;

class Node;
class Database;

class Table
{
private:
	string *path;
	string *nodesPath;

	string *name;
	/* vector<Node *> nodeList; */ // sem uso por enquanto
	vector<tableStruct *> tableHeader;

	Database *parent;
	
public:
	Table(Database *, string *, string *);
	~Table();

	/*
	 *	Will return the path
	 *
	 *	@return path
	 */
	string *getPath();

	/*
	 *	Will return the name
	 *
	 *	@return name
	 */
	string *getName();

	/*
	 *	Will return the header
	 *
	 *	@return header
	 */
	vector<tableStruct *> *getTableHeader();

	/* vector<Node *> *getNodeList(); */ // no use

	/*
	 *	Will return the field index
	 *
	 *	@param field
	 *	@return (exists) ? index : -1
	 */
	int hasField(string *);

	/*
	 *	Will return the field type
	 *
	 *	@param field
	 *	@return (exists) ? type : NULL
	 */
	char *fieldType(string *);

	/*
	 *	Will return the field size
	 *
	 *	@param field
	 *	@return (exists) ? size : 0
	 */
	int fieldSize(string *);

	/*
	 *	Will return how many fields exists
	 *
	 *	@return count
	 */
	int countFields();

	/* void loadState(); */
	/* void saveState(); */

	/*
	 *	Will try to append node in .dat
	 *
	 *	@param node
	 */
	void appendNode(Node *);

	/*
	 *	Will try to read the table header
	 *
	 *	@param name
	 *	@param content
	 *	@return success|fail
	 */
	bool readTable(string, string); // no use

	/*
	 *	Will try to instantiate a new node
	 *
	 *	@return node
	 */
	Node *newNode();
};

#endif
