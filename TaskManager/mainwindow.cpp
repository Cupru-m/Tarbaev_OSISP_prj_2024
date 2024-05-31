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
    menuActions.showChildren = menu->addAction("Показать связанные потоки");
    menuActions.showMoreInfo = menu->addAction("Показать больше информации");
    menuActions.killprocess = menu->addAction("Завершить процесс");
    menuActions.sendSignal = menu->addAction("Отправить сигнал");

    //signalMenu
    signalMenu = new QMenu(this);
    signalActions.CONT = signalMenu->addAction("Отправить сингал CONT");
    signalActions.HUP =signalMenu->addAction("Отправить сингал HUP");
    signalActions.INT = signalMenu->addAction("Отправить сингал INT");
    signalActions.KILL = signalMenu->addAction("Отправить сингал KILL");
    signalActions.STOP = signalMenu->addAction("Отправить сингал Stop");
    signalActions.USER1 =signalMenu->addAction("Отправить сингал USER1");
    signalActions.USER2 = signalMenu->addAction("Отправить сингал USER2");


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

void MainWindow::showSignals(QModelIndex index)
{

    connect(signalActions.CONT, &QAction::triggered, [this, index] { handleCONT(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.HUP, &QAction::triggered, [this, index] { handleHUP(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.INT, &QAction::triggered, [this, index] { handleINT(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.KILL, &QAction::triggered,  [this, index] { handleKILL(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.STOP, &QAction::triggered,  [this, index] { handleSTOP(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.USER1, &QAction::triggered,  [this, index] { handleUSER1(index.data(Qt::UserRole).value<Status>()); });
    connect(signalActions.USER2, &QAction::triggered,  [this, index] { handleUSER2(index.data(Qt::UserRole).value<Status>()); });
    signalMenu->exec();
    updateData();
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    m_updateTimer->stop();
    QModelIndex index = ui->tableView->indexAt(pos);
    if (index.isValid()){

        connect(menuActions.showChildren,&QAction::triggered,[this,index](){
            showChildrenThreads(index);
        });

        connect(menuActions.killprocess, &QAction::triggered, [this, index]() {
            handleKillProcess(index);});

        connect(menuActions.showMoreInfo,&QAction::triggered,[this,index](){
            showMoreInfo(index); });
        connect(menuActions.sendSignal,&QAction::triggered,[this,index](){
            showSignals(index);
        });
        menu->popup(ui->tableView->mapToGlobal(pos));
    }
    m_updateTimer->start();
}

void MainWindow::handleKillProcess(QModelIndex index)
{
    m_updateTimer->stop();
    Status status = index.data(Qt::UserRole).value<Status>();
    kill(status.PID.toInt(),SIGTERM);
    model->removeRow(index.row());
    ui->tableView->setModel(model);
    m_updateTimer->start();
}
void MainWindow::showChildrenThreads(QModelIndex index)
{
    m_updateTimer->stop();
    Status status = index.data(Qt::UserRole).value<Status>();
    QList<Process> childThreads = processManager.getChildeThreads(status.PID);
    for(int i = 0;i < childThreads.size();i++)
    {
        const Process& childThread = childThreads[i];
        Status childStatus = childThread.getStatus();
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
    //ui->tableView->update();
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
void MainWindow::handleCONT(const Status& status)
{
    // Обработка сигнала CONT
    kill(status.PID.toInt(), SIGCONT);
}

void MainWindow::handleHUP(const Status& status)
{
    // Обработка сигнала HUP
    kill(status.PID.toInt(), SIGHUP);
}

void MainWindow::handleINT(const Status& status)
{
    // Обработка сигнала INT
    kill(status.PID.toInt(), SIGINT);
}

void MainWindow::handleKILL(const Status& status)
{
    // Обработка сигнала KILL
    kill(status.PID.toInt(), SIGKILL);
}

void MainWindow::handleSTOP(const Status& status)
{
    // Обработка сигнала STOP
    kill(status.PID.toInt(), SIGSTOP);
}

void MainWindow::handleUSER1(const Status& status)
{
    // Обработка сигнала USER1
    kill(status.PID.toInt(), SIGUSR1);
}

void MainWindow::handleUSER2(const Status& status)
{
    // Обработка сигнала USER2
    kill(status.PID.toInt(), SIGUSR2);
}
