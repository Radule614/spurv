//Autor: Rade Stojanović Datum: 31.05.2020.
#pragma once

#include <string>
#include <sstream>

#include "IR.h"
#include "SyntaxAnalysis.h"

//ovo je Printer klasa koja samo sluzi lijepom ispisu u konzolu i fajl
class Printer {
private:
	Variables m_vars;
	Instructions m_ins;
	Labels m_labels;

	//koristio sam stringstream u koji se gura string reprezentacija svake instrukcije i memorijske promjenljive
	//za ispis instrukcija sam preklapao operator ispisa u IR.h
	std::stringstream m_ss;
public:
	Printer(SyntaxAnalysis& syntax);

	//getter za stringstream (koji kasnije ubacujem u .s fajl)
	std::stringstream* getStream();

	//init funkcija u kojoj se formira stringstream
	void init();

	//funkcija ispisa stringstream-a na konzolu
	void print();
};