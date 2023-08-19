//Autor: Rade Stojanović Datum: 31.05.2020.

#include "SyntaxAnalysis.h"

#include <iostream>
#include <iomanip>

using namespace std;

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()), varCurrentPos(0), insCurrentPos(0)
{
}


bool SyntaxAnalysis::Do()
{
	cout << "Syntax analysis started!\n";

	currentToken = getNextToken();
	
	Q();

	if (!errorFound) 
	{
		cout << "Syntax OK" << endl;
	}
	else {
		printSyntaxError(currentToken);
	}
	
	return !errorFound;
}

Instructions SyntaxAnalysis::getInstructions() 
{
	return m_ins;
}

Variables SyntaxAnalysis::getVariables()
{
	return m_vars;
}

Labels SyntaxAnalysis::getLabels()
{
	return m_labels;
}

void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}


void SyntaxAnalysis::printTokenInfo(Token token)
{
	cout << setw(20) << left << token.tokenTypeToString(token.getType());
	cout << setw(25) << right << token.getValue() << endl;
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound == false)
	{
		if (currentToken.getType() == t)
		{
			//cout << currentToken.getValue() << endl;
			if (currentToken.getType() == T_END_OF_FILE) return;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end()) 
		throw runtime_error("End of input file reached");
	
	return *tokenIterator++;
}

Variable* SyntaxAnalysis::createVariable(VariableType type, std::string name) 
{
	Variable* var = new Variable(type, name, varCurrentPos);
	m_vars.push_back(var);
	varCurrentPos++;
	return var;
}

Instruction* SyntaxAnalysis::createInstruction(InstructionType type) 
{
	Instruction* in = new Instruction(insCurrentPos, type);
	m_ins.push_back(in);
	insCurrentPos++;
	return in;
}

Label* SyntaxAnalysis::createLabel(int position = -1, std::string name = "") 
{
	Label* label = new Label(position, name);
	m_labels.push_back(label);
	return label;
}

Variable* SyntaxAnalysis::getVariable(std::string name, bool definition = false) const
{
	auto var_it = m_vars.begin();
	while (var_it != m_vars.end())
	{
		if ((*var_it)->getName() == name)
		{
			return *var_it;
		}
		var_it++;
	}
	if (definition)
	{
		return nullptr;
	}
	else 
	{
		cout << "ERROR: variable: " << name << " doesn't exist" << endl;
		_Exit(0);
	}
}

Label* SyntaxAnalysis::searchLabels(std::string name) const
{
	for (auto it = m_labels.begin(); it != m_labels.end(); it++)
	{
		if ((*it)->m_name == name)
		{
			return (*it);
		}
	}
	return nullptr;
}

void SyntaxAnalysis::Q()
{
	if (!errorFound) 
	{
		S();
		eat(T_SEMI_COL);
		L();
	}
}


void SyntaxAnalysis::S()
{
	if (!errorFound) 
	{
		if (currentToken.getType() == T_MEM) 
		{
			eat(T_MEM);
			if (currentToken.getValue()[0] != 'm')
			{
				cout << "ERROR: memory variable must start with 'm' " << endl;
				_Exit(0);
			}
			if (getVariable(currentToken.getValue(), true) != nullptr)
			{
				cout << "ERROR: variable: " << currentToken.getValue() << " already defined" << endl;
				_Exit(0);
			}
			Variable* var = createVariable(MEM_VAR, currentToken.getValue());
			eat(T_M_ID);
			var->setValue(atoi(currentToken.getValue().c_str()));
			eat(T_NUM);
		}
		else if (currentToken.getType() == T_REG)
		{
			eat(T_REG);
			if (currentToken.getValue()[0] != 'r')
			{
				cout << "ERROR: register variable must start with 'r' " << endl;
				_Exit(0);
			}
			if (getVariable(currentToken.getValue(), true) != nullptr)
			{
				cout << "ERROR: variable: " << currentToken.getValue() << " already defined" << endl;
				_Exit(0);
			}
			createVariable(REG_VAR, currentToken.getValue());
			eat(T_R_ID);
		}
		else if (currentToken.getType() == T_FUNC)
		{
			Label* label;
			eat(T_FUNC);
			if ((label = searchLabels(currentToken.getValue())) != nullptr)
			{
				if (label->m_position != -1)
				{
					cout << "ERROR: label: " << label->m_name << " already defined" << endl;
					_Exit(0);
				}
				label->m_position = insCurrentPos;
			}
			else
			{
				label = createLabel(insCurrentPos);
				label->m_name = currentToken.getValue();
			}
			
			eat(T_ID);
		}
		else if (currentToken.getType() == T_ID)
		{
			eat(T_ID);
			eat(T_COL);
			E();
		}
		else 
		{
			E();
		}
	}
}


void SyntaxAnalysis::L()
{
	if (!errorFound)
	{
		if (currentToken.getType() == T_END_OF_FILE) 
		{
			eat(T_END_OF_FILE);
		}
		else {
			Q();
		}
	}
}


void SyntaxAnalysis::E()
{
	if (!errorFound)
	{
		if (currentToken.getType() == T_ABS)
		{
			Instruction* in = createInstruction(I_ABS);
			Variables dst;
			Variables src;
			eat(T_ABS);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_ADD)
		{
			Instruction* in = createInstruction(I_ADD);
			Variables dst;
			Variables src;

			eat(T_ADD);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_SUB)
		{
			Instruction* in = createInstruction(I_SUB);
			Variables dst;
			Variables src;

			eat(T_ADD);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_NEG)
		{
			Instruction* in = createInstruction(I_NEG);
			Variables dst;
			Variables src;

			eat(T_NEG);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_XOR)
		{
			Instruction* in = createInstruction(I_XOR);
			Variables dst;
			Variables src;

			eat(T_XOR);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_SLL)
		{
			Instruction* in = createInstruction(I_SLL);
			Variables dst;
			Variables src;

			eat(T_SLL);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if(currentToken.getType() == T_JR)
		{
			Instruction* in = createInstruction(I_JR);
			Variables dst;
			Variables src;

			eat(T_JR);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_LA)
		{
			Instruction* in = createInstruction(I_LA);
			Variables dst;
			Variables src;
			eat(T_LA);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_M_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_LW)
		{
			Instruction* in = createInstruction(I_LW);
			Variables dst;
			Variables src;
			eat(T_LW);
			dst.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			in->m_memoryOffset = atoi(currentToken.getValue().c_str());
			eat(T_NUM);
			eat(T_L_PARENT);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_R_PARENT);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_LI)
		{
			eat(T_LI);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
		}
		else if (currentToken.getType() == T_SW)
		{
			eat(T_SW);
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			eat(T_L_PARENT);
			eat(T_R_ID);
			eat(T_R_PARENT);
		}
		else if (currentToken.getType() == T_B)
		{
			Instruction* in = createInstruction(I_B);
			Variables dst;
			Variables src;

			eat(T_B);
			Label* label;
			if ((label = searchLabels(currentToken.getValue())) != nullptr)
			{
				in->m_jumpDst = label;
			}
			else
			{
				label = createLabel();
				label->m_name = currentToken.getValue();
				in->m_jumpDst = label;
			}
			eat(T_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_BLTZ)
		{
			Instruction* in = createInstruction(I_BLTZ);
			Variables dst;
			Variables src;
			eat(T_BLTZ);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			// label search
			Label* label;
			if ((label = searchLabels(currentToken.getValue())) != nullptr)
			{
				in->m_jumpDst = label;
			}
			else
			{
				label = createLabel();
				label->m_name = currentToken.getValue();
				in->m_jumpDst = label;
			}
			eat(T_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else if (currentToken.getType() == T_BNE)
		{
			Instruction* in = createInstruction(I_BNE);
			Variables dst;
			Variables src;
			eat(T_BNE);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(getVariable(currentToken.getValue()));
			eat(T_R_ID);
			eat(T_COMMA);
			// label search
			Label* label;
			if ((label = searchLabels(currentToken.getValue())) != nullptr)
			{
				in->m_jumpDst = label;
			}
			else
			{
				label = createLabel();
				label->m_name = currentToken.getValue();
				in->m_jumpDst = label;
			}
			eat(T_ID);

			in->setDst(dst);
			in->setSrc(src);
		}
		else 
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}