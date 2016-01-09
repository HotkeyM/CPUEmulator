#include <string.h>
#include <string>
#include <map>
#include "mem.h"

using std::string;

enum command
{
   ADD, SUB, XOR, AND, SHR, MOV
        //add, sub, xor, and, shr, mov
};

class CPU
{
public:
	CPU(RAM *Memory);
	~CPU(){};
	void LoadProgramFromFile(char *filename);
	void SetIP(MemAdress m);
	int GetIP();
	void Exec();
	void ParseFile(char *filename);
	RAM *GetRam(void)
	{
		return this->ram;
	}
	Registers GetRegs(void)
	{
		return this->regs;
	}
	void SetTable(OperatorsTableBasic *t)
	{
		delete table;
		table = t;
	};
private:
	void ParseLineReg(char *line);
	void ParseLineRegMem(char *line);//!!
	void ParseLineTran(char *line);//!!
	void ParseLineNeposr(char *line);
	void ParseLineCompare(char *line);
	void ParseLineIf(char *line);
	void ParseLineDeclare(char *line){};
	void ScanForVariables(string &str)
	{
	//	for (std::map<string,MemAdress>::iterator i = variables.begin(); i != variables.end(); ++i)
		//{
	//		if((*i).first.
	//	}
	}
	RAM *ram;
	Registers regs;
	OperatorsTableBasic *table;
	std::map<string,MemAdress> variables;
	int IP;
	bool CompareMask(string str1, string str2)
	{
		const char simbol = 'a';
		int k=str1.find(".");
	while (k!=string::npos){
		if (str2.at(k)!='.') str1.at(k)=str2.at(k); 													
		else{
			str1.at(k)=simbol;
			str2.at(k)=simbol;
		}
		k=str1.find('.');
	}
	k=str2.find(".");
	while (k!=string::npos){
		if (str1.at(k)!='.') str2.at(k)=str1.at(k);
		else{
			str1.at(k)=simbol;
			str2.at(k)=simbol;
		}
		k=str2.find('.');
	}
	//if (str1.size() > str2.size) str1.resize(str2.size);
	//else str2.resize(str1.size);
	if (str1.size()!=str2.size()) str1.resize(str2.size());
	if (!str1.compare(str2)) return true;
	else return false;
	}
};




class Operator
{
public:
	 Operator();
	 virtual ~Operator(){}
	char *GetLabel();
	virtual void Execute(RAM*, Registers*) = 0;
	virtual char* GetType() = 0;
	void SetLabel(char* l);
private:
	char *label;
	int code;
};

class OperatorReg : public Operator
{
public:
	OperatorReg(){};
	virtual ~OperatorReg(){};
	virtual void Execute(RAM*, Registers*);
	void SetOperand1(RegNum r)
	{
		Operand1 = r;
	}
	void SetOperand2(RegNum r)
	{
		Operand2 = r;
	}
	void SetCommand(command com);
	command GetCommand(void);
	RegNum GetOperand1();
	RegNum GetOperand2();
	char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"Register operator");
		return a;
	}
private:
	RegNum Operand1, Operand2;
	command com;
};

class OperatorRegMem : public Operator
{
public:
	OperatorRegMem(){};
	virtual ~OperatorRegMem(){};
	virtual void Execute(RAM*, Registers*);
	void SetOperandReg(RegNum r)
	{
		OperandReg = r;
	}
	void SetOperandMem(MemAdress m)
	{
		OperandMem = m;
	}
	void SetRegToMem(bool f)
	{
		this->regtomem = f;
	}
	bool GetRegToMem()
	{
		return regtomem;
	}
	RegNum GetOperandReg()
	{
		return OperandReg;
	}
	MemAdress GetOperandMem();
	 char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"Memory-Register operator");
		return a;
	}
	 void SetCommand(command c)
	 {
		 com = c;
	 }
	 command GetCommand(void)
	 {
		 return com;
	 }
private:
	bool regtomem;
	RegNum OperandReg;
	MemAdress OperandMem;
	command com;
};

class OperatorTrans: public Operator
{
public:
    OperatorTrans(){}
    virtual ~OperatorTrans(){}
	virtual void Execute(RAM* ram, Registers* reg);
	void SetOperandReg(RegNum r)
	{
		this->reg = r;
	}
	RegNum GetOperandReg(void)
	{
		return reg;
	}
	void SetLink(MemAdress a)
	{
		this->Link = a;
	}
	MemAdress GetLink()
	{
		return Link;
	}
	char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"Transfer operator");
		return a;
	}
private:
	RegNum reg;
	MemAdress Link;
};

class OperatorCompare: public Operator
{
public:
	OperatorCompare(){};
	virtual ~OperatorCompare(){};
	char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"Comparing operator");
		return a;
	}
	virtual void Execute(RAM *r, Registers *re)
	{
		re->SetRegister(CMP, re->GetRegister(Operand1)-re->GetRegister(Operand2));
	}
	void SetOperand1(RegNum n)
	{
		Operand1 = n;
	}
	void SetOperand2(RegNum n)
	{
		Operand2 = n;
	}
	RegNum GetOperand1()
	{
		return Operand1;
	}
	RegNum GetOperand2()
	{
		return Operand2;
	}
private:
	RegNum Operand1, Operand2;
};

class OperatorIf: public Operator
{
    OperatorIf(){}
    virtual ~OperatorIf(){}
	virtual void Execute(RAM* ram, Registers* reg)
	{
		if (reg->GetRegister(CMP) == 0) reg->IP += reg->GetRegister(this->reg);
	}
	void SetOperandReg(RegNum r)
	{
		this->reg = r;
	}
	RegNum GetOperandReg(void)
	{
		return reg;
	}
	void SetLink(MemAdress a)
	{
		this->Link = a;
	}
	MemAdress GetLink()
	{
		return Link;
	}
	char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"If operator");
		return a;
	}
private:
	RegNum reg;
	MemAdress Link;
};

class OperatorDecl: public Operator
{
public:
    OperatorDecl(){}
    virtual ~OperatorDecl(){}
	virtual void Execute(RAM* r, Registers* re)
	{
		if (ramorreg)
		{
			r->WriteRAM(data, Link);
		}
		else
		{
			re->SetRegister(reg, data);
		}
	}
	void SetRamOperand(MemAdress adr)
	{
		ramorreg = true;
		Link = adr;
	}
	void SetRegOperand(RegNum r)
	{
		ramorreg = false;
		reg = r;
	}
	MemAdress GetLink()
	{
		return Link;
	}
	char* GetType()
	{
		char* a;
		a = new char[32];
		strcat(a,"Declare operator");
		return a;
	}
	void SetOperand(int d)
	{
		data = d;
	}
	int GetOperand(void)
	{
		return data;
	}
private:
	bool ramorreg;
	RegNum reg;
 MemAdress Link;
	int data;
};
