//Autor: Rade Stojanović Datum: 31.05.2020.
#pragma once

#include "IR.h"
#include "SyntaxAnalysis.h"
#include <list>

typedef struct 
{
	Variables* variables;	///< all variables from instructions
	bool** values;			///< matrix value
	int size;				///< size of square matrix
}InterferenceGraph;

class ResourceAllocation {
private:
	Variables m_regVars;
	Instructions m_ins;
	InterferenceGraph* m_ig;
	list<Variable*> simplificationList;
public:
	ResourceAllocation(SyntaxAnalysis& syntax);
	//potreban mi je destruktor radi oslobadjanja memorije jer sam m_ig pravio na heap-u
	~ResourceAllocation();

	//init funkcija koja mi sluzi samo da pozove sve funkcije sa jednog mjesta (pokrenuta je u konstruktoru)
	void init();

	//utility funkcije koje sluze da se formiraju sva polja klase radi zadnjeg koraka algoritma
	void printRegVars() const;
	void printInterferenceGraph() const;
	int getPositionInGraph(Variable* var);
	void computeInterferenceGraph();
	void interferenceClear(Variable* var);
	void simplify();

	//zadnji korak kojim se dodjeljuju registri svakoj od registarskih promjenljivih
	bool allocate();
};