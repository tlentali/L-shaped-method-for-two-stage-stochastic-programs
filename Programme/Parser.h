/*
 * Parser.h
 *
 *  Created on: 11 déc. 2015
 *      Author: haboud910e
 */

#ifndef PARSER_H_
#define PARSER_H_
using namespace std;
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Parser {
public:
	Parser(string filename);
	float nbC;
	float nbS;
	float D;
	vector<vector<float> > ds;
	vector<vector<float> > cmd;
	virtual ~Parser();
};



#endif /* PARSER_H_ */
