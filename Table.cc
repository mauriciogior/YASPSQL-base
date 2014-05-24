
/**
	Table.cc

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Table.h"

using namespace std;

Table::Table(Database *parent, string *name, string *path)
{
	this->name = name;
	this->path = path;
	this->nodesPath = new string();

	// (eg. ".dat" instead of ".dt")
	*(this->nodesPath) = this->path->substr(0,this->path->size() - 3);

	this->nodesPath->append(".dat");

	this->parent = parent;

	// table header ".dt"
	ifstream *file = new ifstream(path->c_str(), ios::binary);
	string *content = new string();

	stringstream *splitStream = NULL;
	string *insideContent = new string();

	tableStruct *tableHeaderField;

	int counter;

	while(getline(*file, *content))
	{
		splitStream = new stringstream(*content);

		counter = 0;

		tableHeaderField = (tableStruct *) malloc(sizeof(tableStruct));

		// split with comma delimiter
		while(getline(*splitStream, *insideContent, ','))
		{
			// name,type,size
			// 0....1....2
			switch(counter)
			{
				case 0:
					tableHeaderField->name = (char *) malloc(insideContent->size()*sizeof(char));
					strcpy(tableHeaderField->name, insideContent->c_str());
					break;

				case 1:
					tableHeaderField->type = (char *) malloc(insideContent->size()*sizeof(char));
					strcpy(tableHeaderField->type, insideContent->c_str());
					break;

				case 2:
					tableHeaderField->size = stoi(*insideContent);
					this->tableHeader.push_back(tableHeaderField); // save the field
					break;
			}

			counter ++;
		}
	}

	//this->loadState();

	delete splitStream;
	delete insideContent;
	delete content;
	delete file;
}

Table::~Table()
{
	if(this->name != NULL) delete this->name;
	if(this->path != NULL) delete this->path;
	if(this->nodesPath != NULL) delete this->nodesPath;

	for(int i = 0; i < this->tableHeader.size(); i++)
	{
		if(this->tableHeader.at(i) != NULL) delete this->tableHeader.at(i);
	}

	//for(int i = 0; i < this->nodeList.size(); i++)
	//{
	//	if(this->nodeList.at(i) != NULL) delete this->nodeList.at(i);
	//}
}

bool Table::readTable(string name, string content)
{
	string *filePath = new string(*this->path + "/" + name + ".dt");

	ifstream *checkFile = new ifstream(*filePath);

	if(checkFile->good())
	{
		return false;
	}

	ofstream *file = new ofstream(*filePath, ios::out);

	*file << content;

	file->close();

	delete filePath;
	delete checkFile;
	delete file;

	return true;
}
	
int Table::hasField(string *content)
{
	// STATES
	// -1: does not have that field
	// 0..n: does have and it is at key 'n'

	int returnState = -1;
	int i;

	int check;

	for(i = 0; i < this->tableHeader.size(); i++)
	{
		check = strcmp(this->tableHeader.at(i)->name, content->c_str());

		if(check == 0)
		{
			returnState = i;
			break;
		}
	}

	return returnState;
}
	
char *Table::fieldType(string *content)
{
	// STATES
	// NULL: does not have that field
	// "char"|"bool"|...

	char *returnState = NULL;
	int i;

	int check;

	for(i = 0; i < this->tableHeader.size(); i++)
	{
		check = strcmp(this->tableHeader.at(i)->name, content->c_str());

		if(check == 0)
		{
			returnState = this->tableHeader.at(i)->type;
			break;
		}
	}

	return returnState;
}
	
int Table::fieldSize(string *content)
{
	// STATES
	// 0: does not have that field
	// 1..n: does have and its size is 'n'
	
	int returnState = 0;
	int i;

	int check;

	for(i = 0; i < this->tableHeader.size(); i++)
	{
		check = strcmp(this->tableHeader.at(i)->name, content->c_str());

		if(check == 0)
		{
			returnState = this->tableHeader.at(i)->size;
			break;
		}
	}

	return returnState;
}

vector<tableStruct *> *Table::getTableHeader()
{
	return &(this->tableHeader);
}

Node *Table::newNode()
{
	Node *toReturn = new Node(this);

	//this->nodeList.push_back(toReturn);

	return toReturn;
}

/*void Table::loadState()
{
	ifstream input_file(*(this->nodesPath), ios::binary);

	if(input_file)
	{
		tableField *insideNode = (tableField *) malloc(sizeof(tableField));

		while(input_file)
		{
			input_file.read((char*) insideNode, sizeof(*insideNode));  
		}

		input_file.close();
	}
}

void Table::saveState()
{
	map<string, tableField *>::iterator it;

	ofstream output_file(*(this->nodesPath), ios::binary);
	
	for(int i = 0; i < this->nodeList.size(); i++)
	{
		for(it = this->nodeList.at(i)->fields.begin(); it != this->nodeList.at(i)->fields.end(); it++)
		{
			output_file.write((char*)(it->second), sizeof(tableField));
		}
	}
	output_file.close();
	//		cout << it->first << ":" << (char *)(it->second->value) << endl;
}*/

void Table::appendNode(Node *toAppend)
{
	// iterate over a map
	map<string, tableField *>::iterator it;

	// open the ".dat" file
	ofstream output_file(*(this->nodesPath), ios::binary | ios::app);
	
	for(it = toAppend->fields.begin(); it != toAppend->fields.end(); it++)
	{
		// will write the whole struct (with sizes given)
		// this will serialize and write in binary
		output_file.write((char*)(it->second), sizeof(tableField));
	}

	output_file.close();
}

int Table::countFields()
{
	return this->tableHeader.size();
}
