#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include "QStandardItemModel"
#include "QStandardItem"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OpenButton_clicked()
{

    QStandardItemModel *model = new QStandardItemModel;
        QStandardItem *item;
    QStringList horizontalHeader;
    QStringList TableItem;
    QStringList RowItem;
    QString FileWay=QFileDialog::getOpenFileName();
    ui->FileWaylabel->setText(FileWay);
    QFile file(FileWay);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());
        QTextStream Data(&file);

        QString text= Data.readAll();
        RowItem=text.split("\n");
        for(int i=0;i<RowItem.size();i++)
        {
            QString NewString=RowItem.at(i);
            NewString.remove(0,1);
            NewString.remove(NewString.size()-1,1);
            TableItem=NewString.split("\";\"");

            for(int j=0; j<TableItem.size();j++)
            {

                if (i==0){

                    horizontalHeader.append(TableItem[j]);

                }else{
                    item = new QStandardItem(TableItem[j]);
                    model->setItem(i-1, j-1, item);
                }
            }
        }

            model->setHorizontalHeaderLabels(horizontalHeader);
            ui->tableView->setModel(model);

            ui->tableView->resizeRowsToContents();
            ui->tableView->resizeColumnsToContents();

            file.close();




}
