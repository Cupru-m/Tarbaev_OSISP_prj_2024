#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "process.h"
#include <QObject>
#include <QString>


class ProcessManager : public QObject { // Теперь ProcessManager наследует от QObject
    Q_OBJECT
public:
    ProcessManager(); // Конструктор с опциональным parent
    QList<Process> getProcessList(){return processList;}
    void updateData();
    QList<Process> getChildProcesses(QString parentPid);
private:
    QList<Process> processList;
};

#endif // PROCESSMANAGER_H
