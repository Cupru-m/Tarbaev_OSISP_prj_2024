#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processmanager.h"
#include "qstandarditemmodel.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include "processinfowidget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef struct MenuActions{
    QAction * killprocess;
    QAction * showChildren;
    QAction * showMoreInfo;
}MenuActions;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateTableSize() {
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
    void resizeEvent(QResizeEvent* event) {
        QMainWindow::resizeEvent(event);
        updateTableSize();
    }
   void  showContextMenu(const QPoint& pos);
    void updateProcessTable(ProcessManager& processManager);
    void updateData();
    void handleKillProcess(QModelIndex index);
    void showChildren(QModelIndex index);
    void showMoreInfo(QModelIndex index);

private:
    ProcessInfoWidget* m_processInfoWidget = nullptr;
    MenuActions menuActions;
    QMenu *menu;
    QStandardItemModel * model;
    ProcessManager processManager;
    QTimer* m_updateTimer;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
