//Autor: Rade Stojanović Datum: 31.05.2020.

#include <iostream>
#include <exception>
#include <fstream>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "ResourceAllocation.h"
#include "Printer.h"

using namespace std;


int main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\simple3.mavn";
		bool retVal = false;

		LexicalAnalysis lex;

		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			//lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syntax(lex);

		if (syntax.Do())
		{
			cout << "Syntax analysis finished successfully!" << endl;

			cout << "Liveness analysis started" << endl;
			Instructions ins = syntax.getInstructions();
			LivenessAnalysis liveness(syntax);
			liveness.init();
			cout << "Liveness analysis finished successfully!" << endl;
			cout << "Resource allocation started" << endl;
			ResourceAllocation resource(syntax);
			cout << "Resource allocation finished successfully!" << endl;
			
			//for petlja radi printa svih polja instrukcija (debug)
			for (auto it = ins.begin(); it != ins.end(); it++)
			{
				(*it)->printInstructionDev();
			}
			
			Printer printer(syntax);
			
			//print dobijenog koda u konzolu
			printer.print();


			//prebacivanje dobijenog koda u .s fajl
			
			const char* outputPath = "./asembler.s";
			ofstream file(outputPath);
			if (file.is_open())
			{
				stringstream* ss = printer.getStream();
				file << ss->str();
			}
			else
			{
				cout << "file error" << endl;
			}
			file.close();
		}
		else
		{
			cout << "Syntax analysis failed!" << endl;
		}
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
