/*
 * main.cpp
 *
 *  Created on: 11 déc. 2015
 *      Author: haboud910e
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ilcplex/ilocplex.h>
#include "Parser.h"
using namespace std;

ILOLAZYCONSTRAINTCALLBACK3(slave, IloNumVarArray, t, Parser, a, IloBoolVarArray, x){
	IloEnv env = getEnv();
	IloNum l_dual;
	IloNumArray m_dual(env,a.nbC);
	for (int s = 0; s < a.nbS; ++s) {

		try{
			IloNumArray xbis(env, a.nbC);
			getValues(xbis, x);
			IloEnv env2 = IloEnv();
			IloModel m2(env2);
			IloNumVarArray y(env2, a.nbC);
			for (IloInt i = 0; i < a.nbC; ++i) {
				y[i] = IloNumVar(env2,0,1);
			}

			IloExpr expr1(env2);
			IloExprArray expr2(env2, a.nbC);
			for (int i = 0; i < a.nbC; ++i) {
				expr2[i] = IloExpr(env2);
			}

			IloNum membred1;
			IloRangeArray mu(env2,a.nbC);
			IloRange lambda;
			membred1 = a.D;

			for (int i = 0; i < a.nbC; ++i) {
				expr1 += -a.ds[i][s] * y[i];
				membred1 -= a.ds[i][s] * xbis[i];
				expr2[i] = y[i];
				mu[i] = (expr2[i] <= xbis[i]);
				m2.add(mu[i]);
			}

			lambda = (expr1 <= membred1);
			m2.add(lambda);
			IloExpr obj(env2);
			for (int i = 0; i < a.nbC; ++i) {
				obj -= a.cmd[i][1] * y[i];
			}
			m2.add(IloMaximize(env2,obj));
			IloCplex cplex(m2);
			cplex.setOut(env.getNullStream());
			cplex.solve();
			l_dual = cplex.getDual(lambda);
			cplex.getDuals(m_dual,mu);
			IloExpr coupe(getEnv());
			IloExpr sum1(getEnv());
			IloExpr sum2(getEnv());
			cout << cplex.getObjValue()<<endl;
			for (int i = 0; i < a.nbC; ++i) {
				cout<<xbis[i];
			}
			cout<<endl;
			for (int i = 0; i < a.nbC; ++i) {
				cout<<cplex.getValue(y[i])<<" ";
			}
			cout<<endl;
			if (cplex.getObjValue()<getValue(t[s])){
				for (int i = 0; i < a.nbC; ++i) {
					sum1+=a.ds[i][s]*x[i];
					sum2+=m_dual[i]*x[i];
				}
				coupe = (a.D-sum1)*l_dual/(float)a.nbS+sum2/(float)a.nbS;
				IloRange cut;
				cut = (-t[s]+coupe>=0);
				//cout << cut<<endl;
				add(cut);
				//cout<<"hey sa coupe"<<endl;
			}
		} catch (...) {
			throw;
		}
	}
}
ILOLAZYCONSTRAINTCALLBACK3(slaveInteger, IloNumVarArray, t, Parser, a, IloBoolVarArray, x){

	IloEnv env = getEnv();
	for (int s = 0; s < a.nbS; ++s) {
		try{
			IloNumArray xbis(env, a.nbC);
			getValues(xbis, x);
			IloEnv env2 = IloEnv();
			IloModel m2(env2);
			IloBoolVarArray y(env2, a.nbC);
			for (IloInt i = 0; i < a.nbC; ++i) {
				y[i] = IloBoolVar(env2);
			}

			IloExpr expr1(env2);
			IloExprArray expr2(env2, a.nbC);
			for (int i = 0; i < a.nbC; ++i) {
				expr2[i] = IloExpr(env2);
			}

			IloNum membred1;
			IloRangeArray mu(env2,a.nbC);
			IloRange lambda;
			membred1 = a.D;

			for (int i = 0; i < a.nbC; ++i) {
				expr1 += -a.ds[i][s] * y[i];
				membred1 -= a.ds[i][s] * xbis[i];
				expr2[i] = y[i];
				mu[i] = (expr2[i] <= xbis[i]);
				m2.add(mu[i]);
			}

			lambda = (expr1 <= membred1);
			m2.add(lambda);
			IloExpr obj(env2);
			for (int i = 0; i < a.nbC; ++i) {
				obj -= a.cmd[i][1] * y[i];
			}
			m2.add(IloMaximize(env2,obj));
			IloCplex cplex(m2);
			cplex.setOut(env.getNullStream());
			cplex.solve();
			cout << cplex.getObjValue() << endl;
			for (int i = 0; i < a.nbC; ++i) {
				cout << xbis[i];
			}
			cout << endl;
			for (int i = 0; i < a.nbC; ++i) {
				cout << cplex.getValue(y[i]);
			}
			cout << endl;
			IloExpr coupe(getEnv());
			coupe = - t[s];
			int cmp = -1;
			float q = cplex.getObjValue();
			for (int i = 0; i < a.nbC; ++i) {
				if (xbis[i] == 1) {
					cmp++;
					coupe+=x[i]*q;
				} else {
					coupe-=x[i]*q;
				}
			}
			coupe -= q*cmp;
			IloRange cut;
			cut = (coupe>=0);
			add(cut);
			//cout<<"hey sa coupe"<<endl;

		} catch (...) {
			throw;
		}
	}
}

void master(Parser a){
	IloEnv env = IloEnv();
	IloModel m(env);
	try{
		IloBoolVarArray x(env, a.nbC);
		IloNumVarArray t(env,a.nbS,-IloInfinity,0,ILOFLOAT);
		IloExpr sum_t(env);
		for (int i = 0; i < a.nbC; ++i) {
			x[i] = IloBoolVar(env);
		}

		for (int s = 0; s < a.nbS; ++s) {
			sum_t += t[s];
		}
		IloExpr obj(env);
		for (int i = 0; i < a.nbC; ++i) {
			obj+= a.cmd[i][0]*x[i]+ sum_t/(float)a.nbS;
		}
		cout<<endl;
		m.add(IloMaximize(env, obj));
		IloCplex cplex(m);
		cplex.use(slave(env,t,a,x));
		cplex.setOut(env.getNullStream());
		cplex.solve();
		cout<<cplex.getObjValue();
	}catch(IloException& e){
		cerr << "Error"<<e<<endl;

	}
}



int main(){
	Parser a("OA_O5_S3_L0.5_B10_R5_4.txt");
	master(a);
	return 0;

}

