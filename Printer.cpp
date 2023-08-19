//Autor: Rade Stojanović Datum: 31.05.2020.

#include "Printer.h"

using namespace std;

Printer::Printer(SyntaxAnalysis& syntax) : m_ss(""), m_vars(syntax.getVariables()), m_ins(syntax.getInstructions()), m_labels(syntax.getLabels())
{
	init();
}

stringstream* Printer::getStream()
{
	return &m_ss;
}

void Printer::init()
{
	m_ss << ".globl main\n\n";
	m_ss << ".data\n";
	for (auto it = m_vars.begin(); it != m_vars.end(); it++)
	{
		if ((*it)->getType() == MEM_VAR)
		{
			m_ss << (*it)->getName() << ":\t\t\t.word " << (*it)->getValue() << endl;
		}
	}
	m_ss << endl;
	m_ss << ".text" << endl;
	int counter = 0;
	for (auto it = m_ins.begin(); it != m_ins.end(); it++, counter++)
	{
		for (auto it2 = m_labels.begin(); it2 != m_labels.end(); it2++)
		{
			if ((*it2)->m_position == counter)
			{
				m_ss << *(*it2) << endl;
			}
		}
		m_ss << *(*it) << endl;
	}
}

void Printer::print()
{
	cout << endl << endl;
	cout << m_ss.str() << endl << endl;;
}
