#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

struct Status {
    QString PID;
    QString PPID;
    QString Name;
    QString RssAnon;
    QString RssFile;
    QString VmSize;
    QString UID;
    QString UserName;
    QString CPU;
    long long LoadingTime;
};

class Process  {
public:
    Process(const Process& other) :status(other.status){};
    Process(){};
    Process(const QString& entry);
    double getCpuUtilization(int pid,QString dir);
    void updateData(const QString& entry);
    long long getProcessStartTimeFromProc(QString entry);
    // Геттеры
    Status getStatus() const { return status; }
private:

   Status status;
};

#endif // PROCESS_H
