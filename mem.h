#include <vector>
using std::vector;



enum RegNum
{
	AX, BX, CX, DX, SP, BP, SI, DI, CMP
};

typedef int MemAdress ;

class RAM
{
public:
	//RAM(int MemorySize);
	virtual void WriteRAM(int data, int adress)
	{
		ram[adress] = data;
	}
	virtual int ReadRAM(int adress)
	{
		return ram[adress];
	}
	virtual void SetSize(int MemorySize)
	{
		memsize = MemorySize;
		ram = new int[MemorySize];
		for(int i = 0; i<memsize; i++) ram[i] = 0;
	}
private:
	int *ram;
	int memsize;
};

class Registers
{
public:
	void SetRegister(RegNum r, int data);
	int GetRegister(RegNum r);
	int IP;
	void SetBeginning(void)
	{
		IP = 0;
	}

private:
	int regs[8];
};

class Operator;

struct TableElement
{
	MemAdress a;
	Operator *op;
	TableElement* next;
};

class OperatorsTableBasic
{
public:
	virtual ~OperatorsTableBasic(){};
	virtual void AddElement(Operator *op, MemAdress a)=0;
	virtual void InsertElement(Operator *op, MemAdress a)=0;
	virtual void DeleteElement(MemAdress a)=0;
	virtual Operator *GetElement(MemAdress a)=0;
};

class OperatorsTable: public OperatorsTableBasic
{
public:
	OperatorsTable();
	virtual ~OperatorsTable()
	{
		while (table)
		{
			TableElement *t;
			t = table->next;
			delete table;
			table = t;
		}
	};
	virtual void AddElement(Operator *op, MemAdress a);
	virtual void InsertElement(Operator *op, MemAdress a);
	virtual void DeleteElement(MemAdress a);
	virtual Operator *GetElement(MemAdress a); 
private:
	TableElement* table;
};

class OperatorsTableStl: public OperatorsTableBasic
{
public:
	OperatorsTableStl()
	{
		Map.resize(1);
	}
	~OperatorsTableStl()
	{
		for(int i = 0; i <Map.size(); i++)
		{
			delete Map[i];
		}
	}
	virtual void AddElement(Operator *op, MemAdress a)
	{
		if (Map.size() <= a + 1) Map.resize(a+1);
		Map[a] = op;
	}
	virtual void InsertElement(Operator *op, MemAdress a)
	{
		if (Map.size() < a + 1) Map.resize(a+1);
		Map.insert(Map.begin() + a, op);
	
	}
	virtual void DeleteElement(MemAdress a)
	{
		delete Map[a];
		Map.erase(Map.begin() + a);
	}
	virtual Operator *GetElement(MemAdress a)
	{
		if (a < Map.size()) return Map[a];
		else return NULL;
	}
private:
	std::vector<Operator*> Map;
};
