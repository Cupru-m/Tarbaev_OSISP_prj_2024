#include "processmanager.h"
#include "qdir.h"

class FileOpenError : public std::runtime_error {
public:
    explicit FileOpenError(const QString& message) : std::runtime_error(message.toStdString()) {}
};

ProcessManager::ProcessManager()
{
    updateData();
}

QList<Process> ProcessManager::getChildeThreads(QString parentPid)
{
    QList<Process> childrenProcessList;
    QDir childrenDir("/proc/"+parentPid+"/task");
    QStringList entries = childrenDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& entry : entries) // для каждой дирректории /proc/<PID>
    {
        try{
            Process process(childrenDir.path()+ "/"+entry);
            childrenProcessList.append(process);
        }catch(FileOpenError&)
        {
            continue;
        }
    }
    return childrenProcessList;
}
void ProcessManager:: updateData()
{
    processList.clear();
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& entry : entries) // для каждой дирректории /proc/<PID>
    {
        try{
            Process process(procDir.path()+ "/"+entry);
            processList.append(process);
        }catch(FileOpenError&)
        {
            continue;
        }
    }
}
