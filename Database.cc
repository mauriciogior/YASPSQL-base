
/**
	Database.cc

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
#include <stdexcept>

#include "Database.h"

using namespace std;

Database::Database(string *name, string *path)
{
	this->name = name;
	this->path = path;

	DIR *directory = NULL;
	struct dirent *entity = NULL;
	int counter = 0;

	int start;

	string check;

	if((directory = opendir(path->data())) != NULL)
	{
		// read all files within a directory
		while((entity = readdir(directory)) != NULL)
		{
			counter++;

			if(counter <= 2) continue;

			check = entity->d_name;

			start = check.size() - 3;

			try
			{
				check = check.substr(start, start + 2);

				if(check == ".dt") // if it is ".dt", instantiate
				{
					this->addTable(new string(entity->d_name));
				}
			}
			catch(const std::out_of_range& oor) {}
		}

		closedir(directory);
	}
	else
	{
		// could not open directory
		cout << "FAILED" << endl;
	}
}

Database::~Database()
{
	if(this->name != NULL) delete this->name;
	if(this->path != NULL) delete this->path;

	map<string, Table *>::iterator it;

	for(it = this->tableList.begin(); it != this->tableList.end(); it++)
	{
		if(it->second != NULL) delete it->second;
	}
}

void Database::addTable(string *subPath)
{
	// full path (eg. ./banco0/Pessoa.dt)
	string *filePath = new string(*this->path + "/" + *subPath);
	// name (eg. Pessoa)
	string *name = new string(subPath->substr(0, subPath->size() - 3));

	// instantiate a new table
	Table *item = new Table(
						this,
						new string(name->data()), 
						new string(filePath->data())
					 );

	// save in list
	this->tableList[name->data()] = item;

	delete name;
	delete filePath;
	delete subPath;
}

bool Database::createTable(string name, string tableHeader)
{
	// full path (eg. ./banco0/Pessoa.dt)
	string *filePath = new string(*this->path + "/" + name + ".dt");

	ifstream *checkFile = new ifstream(filePath->c_str());

	// check if file exists
	if(checkFile->good())
	{
		return false;
	}

	// create a file
	ofstream *file = new ofstream(filePath->c_str(), ios::binary);

	// write the header
	file->write((char*)tableHeader.data(), tableHeader.size());

	file->close();

	// instantiate
	Table *created = new Table(
						this,
						new string(name.data()),
						new string(filePath->data())
					 );

	// save in the list
	this->tableList[name.data()] = created;

	delete filePath;
	delete checkFile;
	delete file;

	return true;
}
