/*
 * Parser.cpp
 *
 *  Created on: 11 déc. 2015
 *      Author: haboud910e
 */

#include "Parser.h"



Parser::Parser(string filename) {
	// TODO Auto-generated constructor stub
	ifstream fichier(filename.c_str(), ios::in);
	if (fichier) {
		fichier>>nbC>>ws>>nbS>>ws>>D>>ws;
		int id,b,cs,d;

		for (int i = 0; i < nbC; ++i) {
			fichier>>id>>ws>>b>>ws>>cs>>ws;
			cmd.push_back(vector<float>());
			ds.push_back(vector<float>());
			cmd[id].push_back(b);
			cmd[id].push_back(cs);
			for (int j = 0; j < nbS; ++j) {
				fichier>>d>>ws;
				ds[id].push_back(d);
			}
		}

	} else {
		cerr << "Impossible d'ouvrir le fichier !" << endl;
	}

}

Parser::~Parser() {
	// TODO Auto-generated destructor stub
}


