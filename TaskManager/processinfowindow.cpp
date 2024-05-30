// ProcessInfoWindow.cpp
#include "processinfowindow.h"
#include "ui_processinfowindow.h"

ProcessInfoWindow::ProcessInfoWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProcessInfoWindow)
{
    ui->setupUi(this);
}

ProcessInfoWindow::~ProcessInfoWindow()
{
    delete ui;
}

void ProcessInfoWindow::setProcess(const Process &process)
{
    ui->nameLabel->setText(process.getStatus().Name);
    ui->userLabel->setText(process.getUserName());
    ui->rssAnonLabel->setText(process.getStatus().RssAnon);
    ui->cpuLabel->setText(process.getCPU());
    // Добавьте другие элементы пользовательского интерфейса, если необходимо
}
