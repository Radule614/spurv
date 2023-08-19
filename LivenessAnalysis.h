//Autor: Rade Stojanović Datum: 31.05.2020.
#pragma once

#include "IR.h"
#include "SyntaxAnalysis.h"

class LivenessAnalysis {
private:
	Instructions m_ins;
	Variables m_vars;
	
public:
	LivenessAnalysis(SyntaxAnalysis& syntax);

	void init();
	
	//utility funkcije koje su mi bile potrebne da realizujem svaki od koraka Liveness analize
	bool variableExists(Variable* variable, Variables variables);
	bool variablesEqual(Variables vars1, Variables vars2);
	bool instructionsEqual(list<Instruction>& ins);
	Instruction* findInstruction(int position);

	//svaka funckija predstavlja korak liveness analize
	
	//determinePred() odredjuje prethodnike instrukcija
	void determinePred();
	
	//odredjuje nasljednike instrukcija
	void determineSucc();

	//odreduje promjenljive koje su u upotrebi u svakoj instrukciji posebno
	void determineUse();

	//odredjuje sve promjenljive koje se definisu(ili redefinisu) u svakoj instrukciji posebno
	void determineDef();

	//zadnji korak algoritma koji odredjuje koje su promjenljive zive na ulazu i izlazu svake instrukcije
	void determineInAndOut();
};