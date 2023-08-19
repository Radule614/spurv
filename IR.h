//Autor: Rade Stojanović Datum: 31.05.2020.

#ifndef __IR__
#define __IR__

#include "Types.h"
#include <iostream>
#include <string>

using namespace std;

/**
 * This class represents one variable from program code.
 */
enum VariableType
{
	MEM_VAR = 0,
	REG_VAR,
	NO_TYPE
};

class Variable
{
public:
	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(VariableType type, std::string name, int pos) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(VariableType type, std::string name, int pos, int value) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign), m_value(value) {}
	
	//geteri i seteri
	Regs getAssignment() const { return m_assignment; }
	VariableType getType() const { return m_type; }
	std::string getName() const { return m_name; }
	int getValue() const { return m_value; }
	
	bool setValue(int value) 
	{
		m_value = value;
		return true;
	}

	void setAssignment(Regs assignment)
	{
		this->m_assignment = assignment;
	}

	//preklopljeni operator ispisa radi lakseg unosa u stringstream unutar Printer klase
	friend std::ostream& operator<<(std::ostream& out, const Variable& ins)
	{
		if (ins.getType() == MEM_VAR) 
		{
			out << ins.getName();
		}
		else if (ins.getType() == REG_VAR)
		{
			out << "$t" << (int)(ins.getAssignment()-1);
		}
		return out;
	}
private:
	int m_value;
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

//dodata struktura za labele
struct Label {
	int m_position;
	std::string m_name;
	Label() {}
	Label(int position, std::string name) : m_position(position), m_name(name) {}

	//preklopljeni operator ispisa radi lakseg unosa u stringstream unutar Printer klase
	friend ostream& operator<<(ostream& out, const Label& l)
	{
		out << l.m_name << ":";
		return out;
	}
};

typedef std::list<Label*> Labels;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	//sva polja ove klase sam prebacio na public radi lakseg koristenja
	int m_position;
	InstructionType m_type;

	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;

	Label* m_jumpDst;
	int m_memoryOffset;

	Instruction() : m_position(0), m_type(I_NO_TYPE) {}
	Instruction(int pos, InstructionType type, Variables& dst, Variables& src) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src) {}
	Instruction(int pos, InstructionType type) : m_position(pos), m_type(type) {}

	void setDst(Variables& dst)
	{
		m_dst = dst;
	}
	void setSrc(Variables& src)
	{
		m_src = src;
	}

	//preklopljeni operator ispisa radi lakseg unosa u stringstream unutar Printer klase
	friend std::ostream& operator<<(std::ostream& out, const Instruction& ins)
	{
		auto dst_it = ins.m_dst.begin();
		auto src_it = ins.m_src.begin();
		switch (ins.m_type)
		{
		case I_ABS:
			out << "\tabs\t\t" << *(*dst_it) << ", ";
			out << *(*src_it);
			break;
		case I_ADD:
			out << "\tadd\t\t" << *(*dst_it) << ", ";
			out << *(*src_it) << ", ";
			src_it++;
			out << *(*src_it);
			break;
		case I_SUB:
			out << "\tsub\t\t" << *(*dst_it) << ", ";
			out << *(*src_it) << ", ";
			src_it++;
			out << *(*src_it);
			break;
		case I_LA:
			out << "\tla\t\t" << *(*dst_it) << ", ";
			out << *(*src_it);
			break;
		case I_LW:
			out << "\tlw\t\t" << *(*dst_it) << ", ";
			out << ins.m_memoryOffset << "(" << *(*src_it) << ")";
			break;
		case I_JR:
			out << "\tjr\t\t" << *(*src_it);
			break;
		case I_BLTZ:
			out << "\tbltz\t\t" << *(*src_it);
			out << ", " << ins.m_jumpDst->m_name;
			break;
		case I_BNE:
			out << "\tbne\t\t" << *(*src_it) << ", ";
			src_it++;
			out << *(*src_it) << ", " << ins.m_jumpDst->m_name;
			break;
		case I_NEG:
			out << "\tneg\t\t" << *(*dst_it) << ", ";
			out << *(*src_it);
			break;
		case I_XOR:
			out << "\txor\t\t" << *(*dst_it) << ", ";
			out << *(*src_it) << ", ";
			src_it++;
			out << *(*src_it);
			break;
		case I_B:
			out << "\tb\t\t" << ins.m_jumpDst->m_name;
			break;
		case I_SLL:
			out << "\tsll\t\t" << *(*dst_it) << ", ";
			out << *(*src_it) << ", ";
			src_it++;
			out << *(*src_it);
			break;
		default:
			break;
		}

		return out;
	}

	//print za debug
	void printInstructionDev() const
	{
		cout << "position: " << m_position << endl;
		cout << "destinations: ";
		for (auto it = m_dst.begin(); it != m_dst.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "sources: ";
		for (auto it = m_src.begin(); it != m_src.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "use: ";
		for (auto it = m_use.begin(); it != m_use.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "def: ";
		for (auto it = m_def.begin(); it != m_def.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "in: ";
		for (auto it = m_in.begin(); it != m_in.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "out: ";
		for (auto it = m_out.begin(); it != m_out.end(); it++) {
			cout << (*it)->getName() << ", ";
		}
		cout << endl << "succ: ";
		for (auto it = m_succ.begin(); it != m_succ.end(); it++) {
			cout << (*it)->m_position << ", ";
		}
		cout << endl << "pred: ";
		for (auto it = m_pred.begin(); it != m_pred.end(); it++) {
			cout << (*it)->m_position << ", ";
		}
		cout << endl << endl;
	}
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;



#endif
