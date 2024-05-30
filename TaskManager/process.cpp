#include "process.h"
#include <sys/time.h>
#include <fstream>
#include <sys/types.h>
#include <utime.h>
#include <pwd.h>
#include <QFile>

class FileOpenError : public std::runtime_error {
public:
    explicit FileOpenError(const QString& message) : std::runtime_error(message.toStdString()) {}
};


QString getUserNameFromUid(const QString& uidLine) {
    QStringList uidParts = uidLine.split('\t');
    if (uidParts.size() >= 2) {
        bool ok;
        uid_t uid = uidParts[1].toUInt(&ok);
        if (ok) {
            struct passwd* pw = getpwuid(uid);
            if (pw) {
                return QString::fromLocal8Bit(pw->pw_name);
            }
        }
    }
    return QString();
}

double Process::getCpuUtilization(int pid,QString dir) {
    std::ifstream procFile(dir.toStdString() + "/stat");
    if (!procFile.is_open()) {
        return -1.0; // Возвращаем -1.0, если не удалось открыть файл
    }

    std::string line;
    std::getline(procFile, line);
    std::istringstream iss(line);

    // Парсим содержимое файла /proc/<pid>/stat
    int i = 0;
    std::string token;
    while (iss >> token) {
        i++;
        long long utime;
        long long stime;
        if (i == 14) { // 14-й токен - это utime (CPU time used in user mode)
         utime = std::stoll(token);
        }
        if (i == 15) { // 15-й токен - это stime (CPU time used in kernel mode)
             stime = std::stoll(token);

            // Вычисляем общее процессорное время, используемое процессом
            long long totalTime = utime + stime;

            // Получаем общее процессорное время, используемое всеми процессами
            std::ifstream statFile("/proc/stat");
            std::string statLine;
            std::getline(statFile, statLine);
            std::istringstream statIss(statLine);
            std::string cpuToken;
            statIss >> cpuToken;
            long long totalSystemTime = 0;
            for (int j = 0; j < 10; j++) {
                std::string statToken;
                statIss >> statToken;
                totalSystemTime += std::stoll(statToken);
            }

            // Вычисляем процент использования ЦП
            double cpuUtilization = static_cast<double>(totalTime) / totalSystemTime * 100.0;
            return cpuUtilization;
        }
    }

    return -1.0; // Возвращаем -1.0, если не удалось найти необходимые данные
}

Process::Process(const QString& entry)
{
    updateData(entry);
}
void Process::updateData(const QString& entry)
{
    QFile statFile(entry+"/status");
    if (!statFile.open(QFile::ReadOnly | QFile::Text))
    {
        throw FileOpenError("faild to open file:" + statFile.fileName());
    }

    QString statContent = QString::fromUtf8(statFile.readAll());
    statFile.close();


    QStringList lines = statContent.split('\n');
    foreach (const QString& line, lines)
    {
        if (line.isEmpty())
            continue;

        QStringList parts = line.split('\t');
        if (parts.size() < 2)
            continue;

        QString key = parts[0].trimmed();
        QString value = parts[1].trimmed();

        if (key == "Name:")
        {
            status.Name = value;
        }
        else if (key == "Pid:")
        {
            status.PID = value;
        }
        else if (key == "PPid:")
        {
            status.PPID = value;
        }
        else if (key == "RssAnon:")
        {
            status.RssAnon = value;
        }
        else if (key == "RssFile:")
        {
            status.RssFile = value;
        }
        else if(key == "VmSize:")
        {
            status.VmSize = value;
        }
        else if(key == "Uid:")
        {
            bool ok;
            status.UID = value;
            int uid = value.toInt(&ok);
            if(ok)
            {
                struct passwd* pw = getpwuid(uid);
                if(pw)
                {
                    status.UserName =QString::fromLocal8Bit(pw->pw_name);;
                }
            }
        }
        status.LoadingTime = getProcessStartTimeFromProc(entry);
    }

    double cpuUtilization = getCpuUtilization(status.PID.toInt(),entry);
    status.CPU = QString::number(cpuUtilization, 'f', 5);
}
long long Process::getProcessStartTimeFromProc(QString entry) {
    std::ifstream file(entry.toStdString() + "/stat");
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        long long startTime = 0;
        int field = 0;
        while (iss >> token) {
            field++;
            if (field == 22) {
                startTime = std::stoll(token);
                break;
            }
        }
        return startTime;
    }
    return -1;
}
