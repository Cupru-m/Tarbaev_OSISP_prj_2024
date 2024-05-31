#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processmanager.h"
#include "qstandarditemmodel.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include "processinfowidget.h"
#define HANDLE_SIGNAL_SLOTS \
void handleCONT(const Status& status); \
    void handleHUP(const Status& status); \
    void handleINT(const Status& status); \
    void handleKILL(const Status& status); \
    void handleSTOP(const Status& status); \
    void handleUSER1(const Status& status); \
    void handleUSER2(const Status& status);

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef struct MenuActions{
    QAction * killprocess;
    QAction * showChildren;
    QAction * showMoreInfo;
    QAction * sendSignal;
}MenuActions;

typedef struct SignalActions{
    QAction* STOP;
    QAction* HUP;
    QAction* CONT;
    QAction* INT;
    QAction* KILL;
    QAction* USER1;
    QAction* USER2;
}SignalActions;
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
    void showChildrenThreads(QModelIndex index);
    void showMoreInfo(QModelIndex index);
    void showSignals(QModelIndex index);
    HANDLE_SIGNAL_SLOTS
private:

    SignalActions signalActions;
    QMenu* signalMenu;
    ProcessInfoWidget* m_processInfoWidget = nullptr;
    MenuActions menuActions;
    QMenu *menu;
    QStandardItemModel * model;
    ProcessManager processManager;
    QTimer* m_updateTimer;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
