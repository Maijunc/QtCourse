#include "patientview.h"
#include "ui_patientview.h"

PatientView::PatientView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PatientView)
{
    ui->setupUi(this);

    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initPatientModel()){
        ui->tableView->setModel(iDatabase.patientTableModel);
        ui->tableView->setSelectionModel(iDatabase.thePatientSelection);
    }
}

PatientView::~PatientView()
{
    delete ui;
}

void PatientView::on_btAdd_clicked()
{
    int curRow = IDatabase::getInstance().addNewPatient();
    emit goPatientEditView(curRow);
}


void PatientView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchPatient(filter);
}


void PatientView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentPatient();
}


void PatientView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().thePatientSelection->currentIndex();

    if (!curIndex.isValid()) {
        // 如果当前选中的行无效，可以弹出一个提示框
        QMessageBox::warning(this, "警告", "请先选择一个患者进行编辑!");
        return;  // 不执行后续操作
    }

    emit goPatientEditView(curIndex.row());
}

