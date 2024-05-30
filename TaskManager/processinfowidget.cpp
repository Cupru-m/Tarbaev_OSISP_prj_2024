#include "processinfowidget.h"
#include "qboxlayout.h"
#include "ui_processinfowidget.h"

ProcessInfoWidget::ProcessInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProcessInfoWidget)
{
    ui->setupUi(this);

    auto layout = new QVBoxLayout(this);

    // Создание QListView
    m_listView = new QListView(this);
    m_listView->setSelectionMode(QAbstractItemView::NoSelection);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setWordWrap(true);
    // Создание модели данных
    m_model = new QStandardItemModel(this);
    m_listView->setModel(m_model);

    // Добавление виджета в макет
    layout->addWidget(m_listView);
    updateUI();
}
QString formatMilliseconds(long long milliseconds) {
    int hours = static_cast<int>(milliseconds / 3600000);
    int minutes = static_cast<int>((milliseconds % 3600000) / 60000);
    int seconds = static_cast<int>((milliseconds % 60000) / 1000);
    int millis = static_cast<int>(milliseconds % 1000);

    return QString("%1:%2:%3.%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(millis, 3, 10, QChar('0'));
}
void ProcessInfoWidget::updateUI()
{
    m_model->clear();

    // Заполнение модели данными о процессе
    addItem(tr("PID: %1").arg(status.PID));
    addItem(tr("PPID: %1").arg(status.PPID));
    addItem(tr("Name: %1").arg(status.Name));
    addItem(tr("RSS Anonymous: %1").arg(status.RssAnon));
    addItem(tr("RSS File: %1").arg(status.RssFile));
    addItem(tr("VM Size: %1").arg(status.VmSize));
    addItem(tr("UID: %1").arg(status.UID));
    addItem(tr("User Name: %1").arg(status.UserName));
    addItem(tr("CPU: %1").arg(status.CPU));
    addItem(tr("Loading Time: %1").arg(formatMilliseconds(status.LoadingTime)));
    m_listView->update();
}

void ProcessInfoWidget::addItem(const QString& text)
{
    QStandardItem* item = new QStandardItem(text);
    item->setEditable(false);
    m_model->appendRow(item);
}
void ProcessInfoWidget::setStatus(Status status)
{
    this->status=status;
}
ProcessInfoWidget::~ProcessInfoWidget()
{
    delete ui;
}
