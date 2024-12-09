#include "patienteditview.h"
#include "ui_patienteditview.h"

PatientEditView::PatientEditView(QWidget *parent,int index)
    : QWidget(parent)
    , ui(new Ui::PatientEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTableModel;
    dataMapper->setModel(IDatabase::getInstance().patientTableModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbComboSex, tabModel->fieldIndex("SEX"), "currentText");
    dataMapper->addMapping(ui->dbDateEditDOB, tabModel->fieldIndex("DOB"));
    dataMapper->addMapping(ui->dbEditIDCard, tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbSpinHeight, tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbSpinWeight, tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile, tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);
}

PatientEditView::~PatientEditView()
{
    delete ui;
}

void PatientEditView::on_btSave_clicked()
{
    // 这一步是将映射控件的修改提交到模型
    dataMapper->submit();

    // 这一步是将模型的数据提交到数据库
    IDatabase::getInstance().submitPatientEdit();

    emit goPreviousView();
}

void PatientEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertPatientEdit();

    emit goPreviousView();
}



