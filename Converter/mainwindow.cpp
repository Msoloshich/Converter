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

QSqlDatabase dbconnect(QString FileWay){
    QSqlDatabase dbfunct;
    dbfunct=QSqlDatabase::addDatabase("QSQLITE");
    return dbfunct;

}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox->hide();
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


        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(0,"info",file.errorString());
            ui->FileWaylabel->clear();
            model->clear();
            ui->tableView->setModel(model);
            ui->comboBox->clear();
            ui->comboBox->hide();
        } else
        {

            if (FileWay.contains(".csv"))//открытие файла формата CSV
            {
                MainDataType="CSV";
                ui->comboBox->clear();
                ui->comboBox->hide();
                QTextStream Data(&file);

                QString text= Data.readAll();//считываем все данные
                file.close();
                RowItem=text.split("\n");// разбиваем данные по строкам
                for(int i=0;i<RowItem.size()-1;i++)// пробигаем по строками и разбиваем строку разделителем ";"
                {
                    QString NewString=RowItem.at(i);
                    NewString.remove(0,1);
                    NewString.remove(NewString.size()-1,1);
                    TableItem=NewString.split("\";\"");

                    for(int j=0; j<TableItem.size();j++)
                    {

                        if (i==0){

                            horizontalHeader.append(TableItem[j]);//заносим заголовок каждого столбца в "горизонтальную шапку"

                        }else{
                            item = new QStandardItem(TableItem[j]);
                            model->setItem(i-1, j, item); //заносим остальные данные таблицы в модель
                        }
                    }
            }

                model->setHorizontalHeaderLabels(horizontalHeader);//заносим "горизонтальную шапку" в нашу модель
                ui->tableView->setModel(model); // заносим нашу модел в элемент tableView для отображения всех данных

                ui->tableView->resizeRowsToContents();//выравниваем вид данных
                ui->tableView->resizeColumnsToContents();
     }
     else
     {

            MainDataType="SQLite";
            db.close();
            ui->comboBox->clear();
            db=dbconnect(FileWay);//подлючаемся к бд
            db.setDatabaseName(FileWay);
            db.open();

            QStringList TablesNames=db.tables();
            for(int i=0;i<TablesNames.size(); i++) //записываем имена таблиц в элемент comboBox
            {
                ui->comboBox->addItem(TablesNames[i]);
            }
            ui->comboBox->show();



         }
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
    if(ui->FileWaylabel->text()!=""){ //проверяем отрыт ли файл
        if(MainDataType=="CSV"){    // если отрыт файл CSV то влючаем конвертацию в SQLIte
            FileWay=QFileDialog::getSaveFileName();// диалоговое оно сохранения
                db=QSqlDatabase::addDatabase("QSQLITE");// отрываем базу данных(или создаем новую если ее нет)
                db.setDatabaseName(FileWay);
                db.open();
                if(db.open())//если получилось отрыть бд
                {
                    QString TableName=QInputDialog::getText(0,"New Table","Table Name",QLineEdit::Normal,"",&NewTableButtonOk);//диалоговое окно для получения названия новой таблицы в бд
                    if(NewTableButtonOk){

                        for( int i = 0; i < ui->tableView->horizontalHeader()->count(); i++ )
                        {
                            QString HeaderNames = ui->tableView->model()->headerData(i, Qt::Horizontal).toString();// получаем названия столбцов
                            SqlQueryString+=" "+HeaderNames;// создаем строки, чтобы в дальнейшем использовать в SQL запросах
                            AllHeaders+=" "+HeaderNames+",";
                            for( int j = 0; j < ui->tableView->verticalHeader()->count(); j++ )
                            {
                              TextValue = ui->tableView->model()->data(ui->tableView->model()->index(j, i)).toString();//считуем значения по столбцам


                              bool success;// переменная с помощью оторой мы определяем можем ли мы сделать преобразование к определенному типу, это необходимо для SQL запроса создания таблицы
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
                    SqlQueryString.remove(SqlQueryString.size()-1,1);//удаляем лишние символы
                    AllHeaders.remove(AllHeaders.size()-1,1);


                    QSqlQuery query;
                    query.prepare("CREATE TABLE "+TableName+"("+SqlQueryString+");");//подготавливаем запрос создания таблицы

                    query.exec();
                    for( int j = 0; j < ui->tableView->verticalHeader()->count(); j++ )
                    {
                        for( int i = 0; i < ui->tableView->horizontalHeader()->count(); i++ )
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
                qDebug()<<db.lastError().text();//если не получилось подлючить бд
            }
        }
        else// делаем конвертацию в CSV
        {

            QString filename=QFileDialog::getSaveFileName();// диалог сохранения файла
            QFile f(filename+".csv");
                if( f.open( QIODevice::WriteOnly ) )//если получилось открыть файл для сохранения
                {
                    QTextStream ts( &f );// поток записи в файл
                    QStringList strList;
                    //strList << "\" \"";
                    for( int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c )
                        strList << "\""+ui->tableView->model()->headerData(c, Qt::Horizontal).toString()+"\"";//пробегаемся по названия столбцов
                    ts << strList.join( ";" )+"\n";
                    for( int r = 0; r < ui->tableView->verticalHeader()->count(); ++r )
                    {
                        strList.clear();
                        for( int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c )//пробегаемся по данным
                        {
                            strList << "\""+ui->tableView->model()->data(ui->tableView->model()->index(r, c), Qt::DisplayRole).toString()+"\"";
                        }
                        ts << strList.join( ";" )+"\n";
                    }
                    f.close();
                }




        }
   }

}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    model = new QSqlTableModel(this,db);//создаем модель зависищую от бд
    model->setTable(TableName);//заносим модель
    model->select();
    ui->tableView->setModel(model);

}
