// ProcessInfoWindow.h
#ifndef PROCESSINFOWINDOW_H
#define PROCESSINFOWINDOW_H

#include <QWidget>
#include "process.h"

namespace Ui {
class ProcessInfoWindow;
}

class ProcessInfoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessInfoWindow(QWidget *parent = nullptr);
    ~ProcessInfoWindow();

    void setProcess(const Process &process);

private:
    Ui::ProcessInfoWindow *ui;
};

#endif // PROCESSINFOWINDOW_H
