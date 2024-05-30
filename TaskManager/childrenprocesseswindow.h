#ifndef CHILDRENPROCESSESWINDOW_H
#define CHILDRENPROCESSESWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "process.h"

namespace Ui {
class ChildrenProcessesWindow;
}

class ChildrenProcessesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChildrenProcessesWindow(QWidget *parent = nullptr);
    ~ChildrenProcessesWindow();

    void setProcessList(const QList<Process>& processes);

private:
    Ui::ChildrenProcessesWindow *ui;
    QStandardItemModel* model;
};

#endif // CHILDRENPROCESSESWINDOW_H
