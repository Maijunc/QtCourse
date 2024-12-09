#include "idatabase.h"

void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE"); //添加 SQL LITE数据库驱动
    QString aFile = "/Users/maitianjun/Qt/Database/lab3";
    database.setDatabaseName(aFile);

    if(!database.open())
    {
        qDebug() << "failed to open database";

        return;
    }
    else
        qDebug() << "open database is ok";

    // 打开数据表
    // openTable();
}

bool IDatabase::initPatientModel()
{
    patientTableModel = new QSqlTableModel(this, database);
    patientTableModel->setTable("patient");
    patientTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit); //数据保存方式： OnmanualSubmit, OnRowChange
    patientTableModel->setSort(patientTableModel->fieldIndex("name"), Qt::AscendingOrder); //排序
    if(!(patientTableModel->select()))// 查询数据
        return false;

    thePatientSelection = new QItemSelectionModel(patientTableModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTableModel->insertRow(patientTableModel->rowCount(), QModelIndex()); //在末尾添加空的一个记录
    QModelIndex curIndex = patientTableModel->index(patientTableModel->rowCount() - 1, 1); //创建最后一行的ModelIndex

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTableModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));;

    patientTableModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTableModel->setFilter(filter);

    return patientTableModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex(); //获取当前选择单元格的模型索引
    patientTableModel->removeRow(curIndex.row());
    patientTableModel->submitAll();
    patientTableModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTableModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTableModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("select username, password from user where username = :USER");
    query.bindValue(":USER", userName);
    query.exec();
    qDebug() << query.lastQuery() << query.first();

    if( query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if(passwd == password) {
            qDebug() << "login ok";
            return "loginOK";
        } else {
            qDebug() << "wrong password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }

    return "loginOK";
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();
}
