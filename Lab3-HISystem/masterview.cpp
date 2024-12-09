#include "masterview.h"
#include "ui_masterview.h"

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    goLoginView();

    IDatabase::getInstance();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    // qDebug() << "goLoginView";
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomeView()));
}

void MasterView::goWelcomeView()
{
    // qDebug() << "goWelcomeView";
    welcomeView = new WelcomeView(this);
    pushWidgetToStackView(welcomeView);

    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
}

void MasterView::goDoctorView()
{
    // qDebug() << "goDoctorView";
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);


}

void MasterView::goDepartmentView()
{
    // qDebug() << "goDepartmentView";
    departmentView = new DepartmentView(this);
    pushWidgetToStackView(departmentView);
}

void MasterView::goPatientEditView(int rowNum)
{
    // qDebug() << "goPatientEditView";
    patientEditView = new PatientEditView(this, rowNum);
    pushWidgetToStackView(patientEditView);

    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goPatientView()
{
    // qDebug() << "goPatientView";
    patientView = new PatientView(this);
    pushWidgetToStackView(patientView);

    connect(patientView, SIGNAL(goPatientEditView(int)), this, SLOT(goPatientEditView(int)));
}

void MasterView::goPreviousView()
{
    int cnt = ui->stackedWidget->count();

    if(cnt > 1) {
        ui->stackedWidget->setCurrentIndex(cnt - 2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        QWidget *widget = ui->stackedWidget->widget(cnt - 1);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int cnt = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(cnt - 1); //总是显示最新加入的View
    ui->labelTitle->setText(widget->windowTitle());
}

void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int cnt = ui->stackedWidget->count();
    // goPreviousView的时候会先跳到上一个页面再删除当前页面，所以currentChanged是先于cnt--进行的
    // 这里的arg1表示转到新的页面的index，如果没有为-1，所以如果要转到登陆页的时候就不显示
    if(cnt > 1 && arg1)
        ui->btBack->setEnabled(true);
    else
        ui->btBack->setEnabled(false);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if(title == "欢迎"){
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);
    } else
        ui->btLogout->setEnabled(false);
}


void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}

