#ifndef PROCESSINFOWIDGET_H
#define PROCESSINFOWIDGET_H
#include "process.h"
#include "qlistview.h"
#include "qstandarditemmodel.h"
#include <QWidget>

namespace Ui {
class ProcessInfoWidget;
}

class ProcessInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessInfoWidget(QWidget *parent = nullptr);
    ~ProcessInfoWidget();
    void setStatus(Status status);
    void updateUI();
    void addItem(const QString& text);
private:
    Ui::ProcessInfoWidget *ui;
    Status status;
    QListView* m_listView;
    QStandardItemModel* m_model;
};

#endif // PROCESSINFOWIDGET_H
