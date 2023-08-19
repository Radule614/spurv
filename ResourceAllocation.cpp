//Autor: Rade Stojanović Datum: 31.05.2020.

#include "ResourceAllocation.h"

using namespace std;

ResourceAllocation::ResourceAllocation(SyntaxAnalysis& syntax) : m_ins(syntax.getInstructions()), simplificationList()
{
	Variables vars = syntax.getVariables();
	for (auto it = vars.begin(); it != vars.end(); it++)
	{
		if ((*it)->getType() == REG_VAR)
		{
			m_regVars.push_back(*it);
		}
	}
	m_ig = new InterferenceGraph();
	m_ig->variables = &m_regVars;
	m_ig->size = m_regVars.size();
	m_ig->values = new bool* [m_ig->size];
	for (int i = 0; i < m_ig->size; i++)
	{
		m_ig->values[i] = new bool[m_ig->size];
		for (int j = 0; j < m_ig->size; j++)
		{
			m_ig->values[i][j] = false;
		}
	}

	init();
}

ResourceAllocation::~ResourceAllocation()
{
	for (int i = 0; i < m_ig->size; i++)
	{
		delete[] m_ig->values[i];
	}
	delete[] m_ig->values;
	delete m_ig;
}

void ResourceAllocation::init()
{
	computeInterferenceGraph();
	simplify();
	if (!allocate())
	{
		cout << endl << "ERROR: resource allocation failed (too few registers available)" << endl;
		_Exit(0);
	}
	
}

void ResourceAllocation::printRegVars() const
{
	int i = 0;
	for (auto it = m_ig->variables->begin(); it != m_ig->variables->end(); it++, i++)
	{
		cout << i << ": " << (*it)->getName() << " -> assignment (" << (*it)->getAssignment() << ")" << endl;
	}
	cout << endl;
}

void ResourceAllocation::printInterferenceGraph() const
{
	cout << "\t";
	for (int i = 0; i < m_ig->size; i++)
	{
		cout << i << " ";
	}
	cout << "\n\n\n";
	for (int i = 0; i < m_ig->size; i++)
	{
		cout << i << "\t";
		for (int j = 0; j < m_ig->size; j++)
		{
			cout << m_ig->values[i][j] << " ";
		}
		cout << endl;
	}
	cout << "\n\n\n";
}

int ResourceAllocation::getPositionInGraph(Variable* var)
{
	int i = 0;
	for (auto it = m_regVars.begin(); it != m_regVars.end(); it++, i++)
	{
		if ((*it)->getName() == var->getName())
		{
			return i;
		}
	}
	cout << "GRESKA" << endl;
	return -1;
}

void ResourceAllocation::computeInterferenceGraph()
{
	for (auto it = m_ins.begin(); it != m_ins.end(); it++)
	{
		Instruction* instr = *it;
		for (auto varIt = instr->m_def.begin(); varIt != instr->m_def.end(); varIt++)
		{
			for (auto varIt2 = instr->m_out.begin(); varIt2 != instr->m_out.end(); varIt2++)
			{
				if ((*varIt2)->getType() != REG_VAR) continue;
				if ((*varIt2)->getName() != (*varIt)->getName())
				{
					m_ig->values[getPositionInGraph(*varIt)][getPositionInGraph(*varIt2)] = true;
					m_ig->values[getPositionInGraph(*varIt2)][getPositionInGraph(*varIt)] = true;
				}
			}
		}
	}
}

void ResourceAllocation::interferenceClear(Variable* var)
{
	int pos = getPositionInGraph(var);
	for (int i = 0; i < m_ig->size; i++)
	{
		m_ig->values[pos][i] = false;
		m_ig->values[i][pos] = false;
	}
}

void ResourceAllocation::simplify()
{
	for (int i = __REG_NUMBER__ - 1; i >= 0; i--)
	{
		for (auto it = m_regVars.begin(); it != m_regVars.end(); it++)
		{
			int interferenceCount = 0;
			for (int j = 0; j < m_ig->size; j++)
			{
				if (m_ig->values[getPositionInGraph(*it)][j])
				{
					interferenceCount++;
				}
			}
			if (interferenceCount == i)
			{
				interferenceClear(*it);
				if (find(simplificationList.begin(), simplificationList.end(), *it) == simplificationList.end())
				{
					simplificationList.push_back(*it);
				}
			}
		}
	}
	//ponovo formiram matricu jer sam je cistio vadeci svaku promjenljivu na simplificationList-u
	computeInterferenceGraph();
}

bool ResourceAllocation::allocate()
{

	Variables* vars = m_ig->variables;
	Variable* var;
	int size = simplificationList.size();
	bool available[4] = { true, true, true, true };
	while (!simplificationList.empty())
	{
		var = simplificationList.back();
		simplificationList.pop_back();
		for (int i = 0; i < 4; i++)
		{
			available[i] = true;
		}
		auto it = vars->begin();
		for (int i = 0; i < size; i++, it++)
		{
			if (m_ig->values[getPositionInGraph(var)][getPositionInGraph(*it)] == true)
			{
				if ((*it)->getAssignment() != no_assign)
				{
					available[(int)(*it)->getAssignment() - 1] = false;
				}
			}
		}
		for (int i = 0; i < size; i++)
		{
			if (available[i])
			{
				var->setAssignment((Regs)(i + 1));
				break;
			}
		}
	}
	for (auto it = vars->begin(); it != vars->end(); it++)
	{
		if ((*it)->getAssignment() == no_assign)
		{
			return false;
		}
	}
	return true;
}
