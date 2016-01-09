#include "cpu.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

CPU::CPU(RAM *Memory)
{
	if (Memory) ram = Memory;
	else ram = new RAM();
	regs.IP = 0;
	table = new OperatorsTable();
}

void CPU::Exec()
{

	Operator *op;
	regs.IP = 0;
	while(op = table->GetElement(this->regs.IP))
	{
	op->Execute(ram, &regs);
	regs.IP++;
	}
}

void CPU::SetIP(MemAdress m)
{
	IP = m;
}

int CPU::GetIP()
{
	return IP;
}

char *Operator::GetLabel()
{
	return label;
}

void CPU::ParseFile(char *filename)
{
	string str;
	char tmpstr[80];
	ifstream s(filename);
	s.clear();
	while(!s.fail())
	{
		if(s.eof()) break;
		getline(s, str);
		
		if(str.length()<2) continue;
		if(CompareMask(str,"... .X,.X")) 
		{
			ParseLineReg((char*)str.c_str());
			continue;
		}
		if(CompareMask(str,"MOV ..,0d...."))
		{
			ParseLineNeposr((char*)str.c_str());
			continue;
		}
		if((CompareMask(str,"... .X,....") || CompareMask(str,"... ....,.X")))
		{
			ParseLineRegMem((char*)str.c_str());
			continue;
		}
		if(CompareMask(str,"CMP .X,.X"))
		{
			ParseLineCompare((char*)str.c_str());
			continue;
		}
		if(CompareMask(str,"JE .X"))
		{
			ParseLineIf((char*)str.c_str());
			continue;
		}
		if(CompareMask(str,"DECLARE"))
		{
			ParseLineDeclare((char*)str.c_str());
		}
		if(CompareMask(str,"JMP ..")) ParseLineTran((char*)str.c_str());
	/*
	s.getline(tmpstr, 80);
	if(strlen(tmpstr)<2) continue;
	if ((tmpstr[5] == 'X') && (tmpstr[8] == 'X')) ParseLineReg(tmpstr);
	if ((tmpstr[8] != 'd') && ((tmpstr[5] == 'X') ^ (tmpstr[8] == 'X'))) ParseLineRegMem(tmpstr);
	if ((tmpstr[0] == 'J')) ParseLineTran(tmpstr);
	if ((tmpstr[0] == 'M') && (tmpstr[7] == '0') &&(tmpstr[8] == 'd')) ParseLineNeposr(tmpstr);
	*/
	}
}

void Operator::SetLabel(char *l)
{
	if (label) delete[] label;
	label =  new char[strlen(l) + 1];
	memcpy(this->label, l, strlen(l));
}

Operator::Operator()
{
	label = NULL;
}

command OperatorReg::GetCommand()
{
	return com;
}

void OperatorReg::SetCommand(command c)
{
	com = c;
}



//Отсюда и далее - стиль копипаста-но-дзюцу

void OperatorReg::Execute(RAM *ram, Registers *regs)
{
	int a, b, c;
	a = regs->GetRegister(Operand1);
	b = regs->GetRegister(Operand2);
	switch(com)
	{
	case add:
		c = a + b;
		
		break;
	case sub:
		c = a - b;
		
		break;
	case xor:
		c = a ^ b;// ^_^
		
		break;
	case and:
		c = a & b;
		break;
	case mov:
		c = b;
		
	}
	regs->SetRegister(Operand1, c);
}

int Registers::GetRegister(RegNum r)
{
	return regs[r];
}

void Registers::SetRegister(RegNum r, int data)
{
	regs[r] = data;
}


void OperatorRegMem::Execute(RAM *ram, Registers *regs)
{
	int a, b, c;
	if (regtomem)
	{
		a = ram->ReadRAM(OperandMem);
		b = regs->GetRegister(OperandReg);
		switch(com)
		{
		case add:
			c = a + b;
		
			break;
		case sub:
			c = a - b;
		
			break;
		case xor:
			c = a ^ b;// ^_^
		
			break;
		case and:
			c = a & b;
			break;
		case mov:
			c = b;
		}
		ram->WriteRAM(c, OperandMem);
	}
	else
	{
		a = ram->ReadRAM(OperandMem);
		b = regs->GetRegister(OperandReg);
		switch(com)
		{
		case add:
			c = a + b;
		
			break;
		case sub:
			c = b - a;
		
			break;
		case xor:
			c = b ^ a;// ^_^
		
			break;
		case and:
			c = b & a;
			break;
	case mov:
		c = a;
		}
		regs->SetRegister(OperandReg, c);
	}
	
}

MemAdress OperatorRegMem::GetOperandMem(void)
{
	return OperandMem;
}

void OperatorTrans::Execute(RAM *ram, Registers *regs)
{
	regs->IP+=regs->GetRegister(reg);
}

void CPU::ParseLineReg(char *line)
{
	OperatorReg *op;
	op = new OperatorReg();
	switch (line[0])
	{
	case 'A':
		op->SetCommand(add);
		break;
	case 'S':
		op->SetCommand(sub);
		break;
	case 'X':
		op->SetCommand(xor);
		break;
	case 'M':
		op->SetCommand(mov);
	}

	switch (line[4])
	{
		case 'A':
			op->SetOperand1(AX);
			break;
		case 'B':
			op->SetOperand1(BX);
			break;
		case 'C':
			op->SetOperand1(CX);
			break;
		case 'D':
			op->SetOperand1(DX);
	}

	
	switch (line[7])
	{
		case 'A':
			op->SetOperand2(AX);
			break;
		case 'B':
			op->SetOperand2(BX);
			break;
		case 'C':
			op->SetOperand2(CX);
			break;
		case 'D':
			op->SetOperand2(DX);
	}
	if(strlen(line)>10)
	{
		char *s = new char[strlen(line)-9];
		strcpy(s,line+10);
		s[strlen(line)-10] = '\0';
		op->SetLabel(s);
	}
	table->AddElement(op, regs.IP);
	regs.IP++;
}

void CPU::ParseLineRegMem(char *line)
{
	int regpos;
	int mempos;
	OperatorRegMem *op = new OperatorRegMem();

	switch (line[0])
	{
	case 'A':
		op->SetCommand(add);
		break;
	case 'S':
		op->SetCommand(sub);
		break;
	case 'X':
		op->SetCommand(xor);
		break;
	case 'M':
		op->SetCommand(mov);
	}

	if(line[5] == 'X') op->SetRegToMem(false);
	else op->SetRegToMem(true);
	if (!op->GetRegToMem())
	{
		regpos = 4;
		mempos = 7;
	}
	else
	{
		mempos = 4;
		regpos = 9;
	}
		switch (line[regpos])
	{
		case 'A':
			op->SetOperandReg(AX);
			break;
		case 'B':
			op->SetOperandReg(BX);
			break;
		case 'C':
			op->SetOperandReg(CX);
			break;
		case 'D':
			op->SetOperandReg(DX);
	}
		char tmpstr[5];
		strncpy(tmpstr,line+mempos,4);
		tmpstr[4] = '\0';
		op->SetOperandMem(atoi(tmpstr));

		if(strlen(line)>12)
	{
		char *s = new char[strlen(line)-11];
		strcpy(s,line+12);
		s[strlen(line)-12] = '\0';
		op->SetLabel(s);
	}

	table->AddElement(op, regs.IP);
	regs.IP++;
}

void CPU::ParseLineTran(char* line)
{
	OperatorTrans *op = new OperatorTrans();

	switch (line[4])
	{
		case 'A':
			op->SetOperandReg(AX);
			break;
		case 'B':
			op->SetOperandReg(BX);
			break;
		case 'C':
			op->SetOperandReg(CX);
			break;
		case 'D':
			op->SetOperandReg(DX);
	
	}

	if(strlen(line)>7)
	{
		char *s = new char[strlen(line)-6];
		strcpy(s,line+7);
		s[strlen(line)-7] = '\0';
		op->SetLabel(s);
	}

	table->AddElement(op, regs.IP);
	regs.IP++;
}

void CPU::ParseLineNeposr(char *line) //Для непосредственного оператора можно использовать только команду "MOV"
{
	OperatorDecl *op = new OperatorDecl();
	char tmpstr[5];
	strncpy(tmpstr, line + 9, 4);
	tmpstr[4]= '\0';
	op->SetOperand(atoi(tmpstr));
	switch (line[4])
	{
		case 'A':
			op->SetRegOperand(AX);
			break;
		case 'B':
			op->SetRegOperand(BX);
			break;
		case 'C':
			op->SetRegOperand(CX);
			break;
		case 'D':
			op->SetRegOperand(DX);
	}

	if(strlen(line)>14)
	{
		char *s = new char[strlen(line)-13];
		strcpy(s,line+14);
		//s[strlen(line)-14] = '\0';
		op->SetLabel(s);
	}

	table->AddElement(op, regs.IP);
	regs.IP++;

}

void CPU::ParseLineCompare(char *line)
{
	OperatorCompare *op = new OperatorCompare();

	switch (line[4])
	{
		case 'A':
			op->SetOperand1(AX);
			break;
		case 'B':
			op->SetOperand1(BX);
			break;
		case 'C':
			op->SetOperand1(CX);
			break;
		case 'D':
			op->SetOperand1(DX);
	}
	switch (line[7])
	{
		case 'A':
			op->SetOperand2(AX);
			break;
		case 'B':
			op->SetOperand2(BX);
			break;
		case 'C':
			op->SetOperand2(CX);
			break;
		case 'D':
			op->SetOperand2(DX);
	}
	if(strlen(line)>10)
	{
		char *s = new char[strlen(line)-9];
		strcpy(s,line+10);
		s[strlen(line)-10] = '\0';
		op->SetLabel(s);
	}
	table->AddElement(op, regs.IP);
	regs.IP++;
}

void CPU::ParseLineIf(char *line)
{
	OperatorTrans *op = new OperatorTrans();

	switch (line[3])
	{
		case 'A':
			op->SetOperandReg(AX);
			break;
		case 'B':
			op->SetOperandReg(BX);
			break;
		case 'C':
			op->SetOperandReg(CX);
			break;
		case 'D':
			op->SetOperandReg(DX);
	
	}

	if(strlen(line)>6)
	{
		char *s = new char[strlen(line)-5];
		strcpy(s,line+6);
		s[strlen(line)-6] = '\0';
		op->SetLabel(s);
	}

	table->AddElement(op, regs.IP);
	regs.IP++;
}