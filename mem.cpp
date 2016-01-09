#include "mem.h"

OperatorsTable::OperatorsTable()
{
	table = 0;
}

void OperatorsTable::AddElement(Operator *op, MemAdress a)
{
	TableElement *t;
	t = new TableElement;
	t->a = a;
	t->op = op;
	t->next = table;
	table = t;
}

Operator *OperatorsTable::GetElement(MemAdress a)
{
	TableElement *t = table;
	while (t)
	{
		if(t->a == a) return t->op;
		t = t->next;
	}
	return 0;
}
 void OperatorsTable::InsertElement(Operator *op, MemAdress a)
{
	TableElement *t = table;
	while (t)
	{
		if(t->a >= a) t->a += 1;
		t = t->next;
	}

	t = new TableElement;
	t->a = a;
	t->op = op;
	t->next = table;
	table = t;
}

void OperatorsTable::DeleteElement(MemAdress a)
{
	TableElement *t = table;
	if (t->a == a) 
	{
		table = t->next;
		delete t;
		t = table;
		while (t)
		{
		t->a++;
		t = t->next;
		}
		return;
	}
	TableElement *prev = table;
	t = t->next;
	while (t)
	{
		if(t->a == a)
		{
			prev->next = t->next;
			delete t;
			return;
		}
		t = t->next;
		prev = prev->next;
	}
}