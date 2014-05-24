
/**
	Node.cc

	@author Maurício Giordano
	@version 0.0.0.0.0.1 07/03/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>

#include "Node.h"

using namespace std;

Node::Node()
{
	this->parent = NULL;
}

Node::Node(Table *parent)
{
	this->parent = parent;

	vector<tableStruct *> *tableHeader = parent->getTableHeader();

	// will fill all fields with trash content (but right size and type)
	for(int i = 0; i < tableHeader->size(); i++)
	{
		this->fields[tableHeader->at(i)->name] = (tableField *) malloc(sizeof(tableField));
		this->fields[tableHeader->at(i)->name]->size = tableHeader->at(i)->size;

		if(strcmp(tableHeader->at(i)->type, "int") == 0)
		{
			this->setField(tableHeader->at(i)->name, -1);
		}
		else if(strcmp(tableHeader->at(i)->type, "bool") == 0)
		{
			this->setField(tableHeader->at(i)->name, false);
		}
		else if(strcmp(tableHeader->at(i)->type, "double") == 0)
		{
			this->setField(tableHeader->at(i)->name, (float) -1.0);
		}
		else if(strcmp(tableHeader->at(i)->type, "blob") == 0)
		{
			this->setField(tableHeader->at(i)->name, "");
		}
		else if(strcmp(tableHeader->at(i)->type, "char") == 0)
		{
			this->setField(tableHeader->at(i)->name, "");
		}
	}
}

Node::~Node()
{
	// No idea how this method is being accessed (since everything are in heap and I don't use delete)
	// but it is! Trust me!
	
	map<string, tableField *>::iterator it;

	for(it = this->fields.begin(); it != this->fields.end(); it++)
	{
		if(it->second->value != NULL) free(it->second->value);

		free(it->second);
	}
}

map<string, tableField *>* Node::getFields()
{
	return &(this->fields);
}

void *Node::getField(char *field)
{
	return this->fields[field]->value;
}

void Node::setField(char *field, int content)
{
	this->fields[field]->value = (int *) malloc(sizeof(int));

	*(int *)(this->fields[field]->value) = content;
}
void Node::setField(char *field, bool content)
{
	this->fields[field]->value = (bool *) malloc(sizeof(bool));

	*(bool *)(this->fields[field]->value) = content;
}
void Node::setField(char *field, float content)
{
	this->fields[field]->value = (float *) malloc(sizeof(float));

	*(float *)(this->fields[field]->value) = content;
}
void Node::setField(char *field, string *content)
{
	int size = this->fields[field]->size;

	this->fields[field]->value = (char *) malloc(size*sizeof(char));

	strcpy((char *)this->fields[field]->value, content->c_str());
}

void Node::setField(char *field, char *type, string *content)
{
	// this will set a value given by string to a type (wow, confusing haha)

	if(this->fields[field]->value != NULL)
		free(this->fields[field]->value);

	if(strcmp(type, "int") == 0)
	{
		this->setField(field, stoi(*content));
	}
	else if(strcmp(type, "bool") == 0)
	{
		if(strcmp(content->c_str(), "true") == 0)
		{
			this->setField(field, true);
		}
		else
		{
			this->setField(field, false);
		}
	}
	else if(strcmp(type, "double") == 0)
	{
		this->setField(field, stof(*content));
	}
	else if(strcmp(type, "blob") == 0)
	{
		this->setField(field, content);
	}
	else if(strcmp(type, "char") == 0)
	{
		this->setField(field, content);
	}
}
