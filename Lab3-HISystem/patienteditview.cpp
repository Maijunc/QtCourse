#include "patienteditview.h"
#include "ui_patienteditview.h"

PatientEditView::PatientEditView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PatientEditView)
{
    ui->setupUi(this);
}

PatientEditView::~PatientEditView()
{
    delete ui;
}
