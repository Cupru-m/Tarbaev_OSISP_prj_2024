#ifndef UI_PROCESSINFOWINDOW_H
#define UI_PROCESSINFOWINDOW_H

#include <QWidget>

namespace Ui {
class ui_ProcessInfoWindow;
}

class ui_ProcessInfoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ui_ProcessInfoWindow(QWidget *parent = nullptr);
    ~ui_ProcessInfoWindow();

private:
    Ui::ui_ProcessInfoWindow *ui;
};

#endif // UI_PROCESSINFOWINDOW_H
