#include "ui_processinfowindow.h"
#include "ui_ui_processinfowindow.h"

ui_ProcessInfoWindow::ui_ProcessInfoWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ui_ProcessInfoWindow)
{
    ui->setupUi(this);
}

ui_ProcessInfoWindow::~ui_ProcessInfoWindow()
{
    delete ui;
}
