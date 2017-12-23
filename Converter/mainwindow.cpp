#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include "QStandardItemModel"
#include "QStandardItem"
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include "QString"
#include <qsqlrecord.h>
#include <QFileInfo>
#include <QDebug>
#include <QSqlError>
#include "QInputDialog";

QString FileWay="";
QSqlDatabase db;
QString MainDataType;



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

//    QString FileWay=QFileDialog::getOpenFileName(this,tr("Open CSV or Sqlite file"),"/home/bi",tr("CSV files (*.csv);;All files (*.*)"));
    QString FileWay=QFileDialog::getOpenFileName();
    ui->FileWaylabel->setText(FileWay);



    QFile file(FileWay);
     if (FileWay.contains(".csv")){
         MainDataType="CSV";

        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
            QTextStream Data(&file);

            QString text= Data.readAll();
            file.close();
            RowItem=text.split("\n");
            for(int i=0;i<RowItem.size()-1;i++)
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
                        model->setItem(i-1, j, item);
                    }
                }
            }

                model->setHorizontalHeaderLabels(horizontalHeader);
                ui->tableView->setModel(model);

                ui->tableView->resizeRowsToContents();
                ui->tableView->resizeColumnsToContents();
     }
     else
     {
         MainDataType="SQLite";
     }


}

void MainWindow::on_ConvertButton_clicked()
{
    QStringList StoredData;
    QString SqlQueryString;
    QString DataType="INTEGER";
    QString TextValue;
    QString AllHeaders;
    QString StringData;
    bool NewTableButtonOk;
    if(ui->FileWaylabel->text()!=""){
        if(MainDataType=="CSV"){
            FileWay=QFileDialog::getSaveFileName();
                db=QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(FileWay);
                db.open();
                if(db.open())
                {
                    QString TableName=QInputDialog::getText(0,"New Table","Table Name",QLineEdit::Normal,"",&NewTableButtonOk);
                    if(NewTableButtonOk){

                        for( int i = 0; i < ui->tableView->horizontalHeader()->count(); ++i )
                        {
                            QString HeaderNames = ui->tableView->model()->headerData(i, Qt::Horizontal).toString();
                            SqlQueryString+=" "+HeaderNames;
                            AllHeaders+=" "+HeaderNames+",";
                            for( int j = 0; j < ui->tableView->verticalHeader()->count(); ++j )
                            {
                              TextValue = ui->tableView->model()->data(ui->tableView->model()->index(j, i)).toString();


                              bool success;
                              int ConvertIntData =TextValue.toInt(&success);
                              if((success)&&(DataType=="INTEGER"))
                              {
                                  DataType="INTEGER";
                              }
                              else
                              {
                                  double ConvetDoubleData=TextValue.toDouble(&success);
                                  if((success))
                                  {
                                      DataType="REAL";
                                  }
                                  else
                                  {
                                  DataType="TEXT";
                                  break;
                                  }
                               }
                              }
                            SqlQueryString+=" "+DataType+" ,";

                            DataType="INTEGER";
                            }
                    SqlQueryString.remove(SqlQueryString.size()-1,1);
                    AllHeaders.remove(AllHeaders.size()-1,1);


                    QSqlQuery query;
                    query.prepare("CREATE TABLE "+TableName+"("+SqlQueryString+");");

                    query.exec();
                    for( int j = 0; j < ui->tableView->verticalHeader()->count(); ++j )
                    {
                        for( int i = 0; i < ui->tableView->horizontalHeader()->count(); ++i )
                        {
                            TextValue = ui->tableView->model()->data(ui->tableView->model()->index(j, i)).toString();
                            StringData+="'"+TextValue+"' ,";
                        }
                        StringData.remove(StringData.size()-1,1);
                        //ui->FileWaylabel->setText("INSERT INTO "+Fname+"("+AllHeaders+") VALUES("+StringData+");");
                        query.prepare("INSERT INTO "+TableName+"("+AllHeaders+") VALUES("+StringData+");");
                        query.exec();
                        StringData="";

                    }

                    db.close();
                    }
            }
            else
            {
                qDebug()<<db.lastError().text();
            }
        }
   }

}













