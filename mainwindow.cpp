#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RAM *ram = new RAM();
    ram->SetSize(1000);
    cpu = new CPU(ram);
	
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{


    QString fname = QFileDialog::getOpenFileName(this,"Opening");
  
	 QFile file(fname);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     while (!in.atEnd()) {
         QString line = in.readLine();
         this->ui->listWidget->addItem(line);
     }
    file.close();
	cpu->ParseFile((char*)fname.toAscii().constData());
	
	//this->ui->tableWidget->setColumnCount(2);
	//	this->ui->tableWidget->setRowCount(5);

//	ui->tableWidget->itemAt(1,1)->setText("1231");

	QString r[5] = {"AX", "BX", "CX", "DX", "SP"};

	for(int i = 0; i< 5; i++)
	{
		int number = cpu->GetRegs().GetRegister((RegNum)i);
		this->ui->listWidget_2->addItem(r[i] + "  " + QString::number(number));
	}
	
	for (int i = 0; i < 1000; i++)
	{
		int number = cpu->GetRam()->ReadRAM(i);
		this->ui->listWidget_3->addItem(QString::number(i) + ":  " + QString::number(number));
	}
}

void MainWindow::on_startButton_clicked()
{
	QString r[5] = {"AX", "BX", "CX", "DX", "SP"};
	cpu->GetRegs().SetBeginning();
	cpu->Exec();
	ui->listWidget_2->clear();
	ui->listWidget_3->clear();
	for(int i = 0; i< 5; i++)
	{
		int number = cpu->GetRegs().GetRegister((RegNum)i);
		this->ui->listWidget_2->addItem(r[i] + "  " + QString::number(number));
	}
	
	for (int i = 0; i < 1000; i++)
	{
		int number = cpu->GetRam()->ReadRAM(i);
		this->ui->listWidget_3->addItem(QString::number(i) + ":  " + QString::number(number));
	}
}