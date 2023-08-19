//Autor: Rade Stojanović Datum: 31.05.2020.
#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

class SyntaxAnalysis
{
public:

	/**
	* Constructor
	*/
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	* Method which performs lexical analysis
	*/
	bool Do();
	
	// funkcije za kreaciju pokazivaca na strukture koje sam postavljao u odgovaraju listu(polja ove klase)
	// kasnije sam te liste koristio za konstrukciju ostalih objekata(LivenessAnalysis, ResourceAllocation...) u main.cpp
	// nakon zavrsene sintaksne analize
	Variable* createVariable(VariableType type, std::string name);
	Instruction* createInstruction(InstructionType type);
	Label* createLabel(int position, std::string name);
	
	// geteri zbog privatnih polja
	Instructions getInstructions();
	Variables getVariables();
	Labels getLabels();
	
	// utility funkcije koje su mi bile potrebne
	// vraca odgovarajucu promjenljivu iz liste promjenljivih na osnovu imena, ako je ne nadje vraca nullptr
	Variable* getVariable(std::string name, bool definition) const;
	Label* searchLabels(std::string name) const;
private:
	int varCurrentPos;
	int insCurrentPos;
	Variables m_vars;
	Instructions m_ins;
	Labels m_labels;
	
	/**
	* Prints the error message, and token that caused the syntax error
	*/
	void printSyntaxError(Token token);

	/**
	* Prints the token info
	*/
	void printTokenInfo(Token token);

	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);

	/**
	* Returns the next token from the token list
	*/
	Token getNextToken();

	/**
	 * Nonterminal Q
	 */
	void Q();

	/**
	 * Nonterminal S
	 */
	void S();

	/**
	 * Nonterminal L
	 */
	void L();

	/**
	 * Nonterminal E
	 */
	void E();


	/**
	* Reference to lexical analysis module
	*/
	LexicalAnalysis& lexicalAnalysis;

	/**
	* Syntax error indicator
	*/
	bool errorFound;

	/**
	* Iterator to the token list which represents the output of the lexical analysis
	*/
	TokenList::iterator tokenIterator;

	/**
	* Current token that is being analyzed
	*/
	Token currentToken;
};
