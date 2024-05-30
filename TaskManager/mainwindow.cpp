#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QLayout>
#include <qtimer.h>
#include <QScrollBar>
#include <signal.h>
#include "processinfowidget.h"
#define HEADERLABELS {"Имя процесса", "Пользователь", "Память","%ЦП"}
#define UPDATE_TIMER 11000
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
// timer init
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateData);
    m_updateTimer->start(UPDATE_TIMER);
// process info widget

    m_processInfoWidget = new ProcessInfoWidget();

    //model init
    model= new QStandardItemModel(0, 4);
    model->setHorizontalHeaderLabels(HEADERLABELS);
    ui->tableView->setModel(model);

    //menu and menu actions init
    menu = new QMenu(this);
    menuActions.showChildren = menu->addAction("Показать дочерние процессы");
    menuActions.showMoreInfo = menu->addAction("Показать больше информации");
    menuActions.killprocess = menu->addAction("Завершить процесс");

    //table init
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);


    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setWordWrap(false);

     ui->centralwidget->setLayout(new QVBoxLayout());
     ui->centralwidget->layout()->addWidget(ui->tableView);

    this->updateProcessTable(processManager);

}
void MainWindow::updateProcessTable(ProcessManager& processManager) {
    // Получаем список процессов из processManager
    if (model) {
        // Удаляем все строки из модели
        model->removeRows(0, model->rowCount());

        // Обновляем представление таблицы
    for (const Process& process : processManager.getProcessList()) {
        Status status = process.getStatus();
        QList<QStandardItem*> row = {
            new QStandardItem(status.Name),
            new QStandardItem(status.UserName),
            new QStandardItem(status.RssAnon),
            new QStandardItem(status.CPU)
        };

        // Сохраняем указатель на объект Process в пользовательских данных первого элемента строки
        row.first()->setData(QVariant::fromValue(status), Qt::UserRole);
        model->appendRow(row);
    }
    for (int i = 0; i < model->rowCount(); i++) {
        ui->tableView->setRowHeight(i, 20); // Установить высоту строки в 20 пикселей
    }
    ui->tableView->update();
    }
}

void MainWindow::showMoreInfo(QModelIndex index)
{
    Status status = index.data(Qt::UserRole).value<Status>();
    m_processInfoWidget->setStatus(status);
    m_processInfoWidget->updateUI();
    m_processInfoWidget->show();
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    m_updateTimer->stop();
    QModelIndex index = ui->tableView->indexAt(pos);
    if (index.isValid()){

        connect(menuActions.showChildren,&QAction::triggered,[this,index](){
        showChildren(index);
        });

        connect(menuActions.killprocess, &QAction::triggered, [this, index]() {
            handleKillProcess(index);});

        connect(menuActions.showMoreInfo,&QAction::triggered,[this,index](){
            showMoreInfo(index);
    });
    menu->popup(ui->tableView->mapToGlobal(pos));
    }
    m_updateTimer->start();
}

void MainWindow::handleKillProcess(QModelIndex index)
{
     Status status = index.data(Qt::UserRole).value<Status>();
     kill(status.PID.toInt(),SIGTERM);
     model->removeRow(index.row());
     ui->tableView->setModel(model);
}
void MainWindow::showChildren(QModelIndex index)
{
    m_updateTimer->stop();
    Status status = index.data(Qt::UserRole).value<Status>();
    QList<Process> childProcesses = processManager.getChildProcesses(status.PID);
    for(int i = 0;i < childProcesses.size();i++)
    {
        const Process& childProcess = childProcesses[i];
        Status childStatus = childProcess.getStatus();
        QList<QStandardItem*> row = {
            new QStandardItem(childStatus.Name),
            new QStandardItem(childStatus.UserName),
            new QStandardItem(childStatus.RssAnon),
            new QStandardItem(childStatus.CPU)
        };

        // Сохраняем указатель на объект Process в пользовательских данных первого элемента строки
        row.first()->setData(QVariant::fromValue(childStatus), Qt::UserRole);

        for (QStandardItem* item : row) {
            item->setBackground(QBrush(QColor(173, 216, 230)));
        }

        // Вставляем строку дочернего процесса в таблицу
        model->insertRow(index.row() +1 + i, row);
    }
    ui->tableView->update();
    m_updateTimer->start();
}
void MainWindow::updateData()
{
    QScrollBar *scrollBar = ui->tableView->verticalScrollBar();
    int currentScrollPosition = scrollBar->value();
    processManager.updateData();
    updateProcessTable( processManager);
    scrollBar->setValue(currentScrollPosition);
}
MainWindow::~MainWindow()
{
    delete menu;
    delete model;
    delete ui;
}
