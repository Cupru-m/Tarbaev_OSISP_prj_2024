#include "childrenprocesseswindow.h"
#include "ui_childrenprocesseswindow.h"
#include <QVBoxLayout>

ChildrenProcessesWindow::ChildrenProcessesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChildrenProcessesWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel(0, 4);
    model->setHorizontalHeaderLabels(HEADERLABELS);
    ui->tableView->setModel(model);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setWordWrap(false);

    ui->centralwidget->setLayout(new QVBoxLayout);
    ui->centralwidget->layout()->addWidget(ui->tableView);
}

ChildrenProcessesWindow::~ChildrenProcessesWindow()
{
    delete ui;
}

void ChildrenProcessesWindow::setProcessList(const QList<Process>& processes)
{
    model->clear();
    for (const Process& process : processes) {
        Status status = process.getStatus();
        QList<QStandardItem*> row = {
            new QStandardItem(status.Name),
            new QStandardItem(process.getUserName()),
            new QStandardItem(status.RssAnon),
            new QStandardItem(process.getCPU())
        };

        // Сохраняем указатель на объект Process в пользовательских данных первого элемента строки
        row.first()->setData(QVariant::fromValue(status), Qt::UserRole);

        model->appendRow(row);
    }
}
