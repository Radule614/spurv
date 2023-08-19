//Autor: Rade Stojanović Datum: 31.05.2020.

#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"

using namespace std;

LivenessAnalysis::LivenessAnalysis(SyntaxAnalysis& syntax) 
{
	m_ins = syntax.getInstructions();
	m_vars = syntax.getVariables();
}

void LivenessAnalysis::init()
{

	determinePred();
	determineSucc();
	determineUse();
	determineDef();
	determineInAndOut();
}

bool LivenessAnalysis::variableExists(Variable* variable, Variables variables)
{
	for (auto it = variables.begin(); it != variables.end(); it++)
	{
		if ((*it)->getName() == variable->getName())
		{
			return true;
		}
	}
	return false;
}


bool LivenessAnalysis::variablesEqual(Variables vars1, Variables vars2)
{
	for (auto it = vars1.begin(); it != vars1.end(); it++)
	{
		if (!variableExists(*it, vars2))
		{
			return false;
		}
	}
	for (auto it = vars2.begin(); it != vars2.end(); it++)
	{
		if (!variableExists(*it, vars1))
		{
			return false;
		}
	}
	return true;
}

bool LivenessAnalysis::instructionsEqual(list<Instruction>& ins)
{
	for (auto it = m_ins.begin(); it != m_ins.end(); it++)
	{
		for (auto it2 = ins.begin(); it2 != ins.end(); it2++)
		{
			if ((*it)->m_position == it2->m_position)
			{
				if (!variablesEqual((*it)->m_in, it2->m_in) || !variablesEqual((*it)->m_out, it2->m_out))
				{
					return false;
				}
			}
		}
	}
	return true;
}

Instruction* LivenessAnalysis::findInstruction(int position)
{
	for (auto it = m_ins.begin(); it != m_ins.end(); it++)
	{
		if ((*it)->m_position == position)
		{
			return (*it);
		}
	}
	return nullptr;
}

void LivenessAnalysis::determinePred()
{
	if (m_ins.size() == 0)
	{
		return;
	}
	//postavljanje prethodnih instrukcija redom
	auto it = m_ins.begin();
	it++;
	while (it != m_ins.end())
	{
		it--;
		Instruction* pred = (*it);
		it++;
		(*it)->m_pred.push_back(pred);
		it++;
	}

	//postavljanje prethodnih instrukcija prilikom skoka
	it = m_ins.begin();
	while (it != m_ins.end())
	{
		if ((*it)->m_type == I_BLTZ || (*it)->m_type == I_BNE || (*it)->m_type == I_B)
		{
			Label* label = (*it)->m_jumpDst;
			if (label->m_position == -1)
			{
				cout << "ERROR: label: " << label->m_name << " not found" << endl;
				_Exit(0);
			}
			Instruction* succ;
			if ((succ = findInstruction(label->m_position)) != nullptr)
			{
				succ->m_pred.push_back((*it));
			}
		}
		it++;
	}
}

void LivenessAnalysis::determineSucc()
{
	if (m_ins.size() <= 1)
	{
		return;
	}
	//postavljanje narednih instrukcija redom
	auto it = m_ins.begin();
	while (true)
	{
		it++;
		Instruction* succ = (*it);
		it--;
		(*it)->m_succ.push_back(succ);
		it++;
		it++;
		if (it == m_ins.end()) break;
		it--;
	}

	//postavljanje narednih instrukcija ukoliko imamo skok
	it = m_ins.begin();
	while (it != m_ins.end())
	{
		if ((*it)->m_type == I_BLTZ || (*it)->m_type == I_BNE || (*it)->m_type == I_B)
		{
			Label* label = (*it)->m_jumpDst;
			if (label->m_position == -1)
			{
				cout << "ERROR: label: " << label->m_name << " not found" << endl;
				_Exit(0);
			}
			Instruction* succ;
			if ((succ = findInstruction(label->m_position)) != nullptr)
			{
				(*it)->m_succ.push_back(succ);
			}
		}
		it++;
	}
}

void LivenessAnalysis::determineUse()
{
	auto it = m_ins.begin();
	while (it != m_ins.end())
	{
		Variables sources = (*it)->m_src;
		auto it2 = sources.begin();
		while (it2 != sources.end())
		{
			(*it)->m_use.push_back(*it2);
			it2++;
		}
		it++;
	}
}

void LivenessAnalysis::determineDef()
{
	auto it = m_ins.begin();
	while (it != m_ins.end())
	{
		if (!(*it)->m_dst.empty())
		{
			(*it)->m_def.push_back((*it)->m_dst.front());
		}
		it++;
	}
}

void LivenessAnalysis::determineInAndOut()
{
	list<Instruction> temp;
	do{
		temp.clear();
		for (auto rit = m_ins.rbegin(); rit != m_ins.rend(); ++rit) 
		{
			temp.push_back(*(*rit));
			
			for (auto it = (*rit)->m_succ.begin(); it != (*rit)->m_succ.end(); it++)
			{
				for (auto it2 = (*it)->m_in.begin(); it2 != (*it)->m_in.end(); it2++)
				{
					(*rit)->m_out.push_back(*it2);
				}
			}
			(*rit)->m_out.sort();
			(*rit)->m_out.unique();

			for (auto it = (*rit)->m_use.begin(); it != (*rit)->m_use.end(); it++)
			{
				(*rit)->m_in.push_back(*it);
			}
			
			for (auto it = (*rit)->m_out.begin(); it != (*rit)->m_out.end(); it++)
			{
				if (!variableExists(*it ,(*rit)->m_def))
				{
					(*rit)->m_in.push_back(*it);
				}
			}

			
			(*rit)->m_in.sort();
			(*rit)->m_in.unique();
		}
	}
	while (!instructionsEqual(temp));
}
