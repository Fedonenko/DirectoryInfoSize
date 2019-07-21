#ifndef FILEINFO_H
#define FILEINFO_H

#include<QObject>
#include<QApplication>
#include<QString>
#include<QMap>
#include<QDir>

struct StatisticFiles;

class FileInfo : public QObject
{
    Q_OBJECT
private:
    int maxPrBar;
    int valuePrBar;
    StatisticFiles *StatisticAllFiles;
    QMap<QString, StatisticFiles> sizeFiles;
    const QDir dir;


public:
    FileInfo( QDir dir = QApplication::applicationDirPath(), QObject *parent = Q_NULLPTR);
    ~FileInfo();
private:
    void start(const QDir&);

    QString fileSize(quint64);
signals:
    void prBarInf(int, int);
    void endFileInfo( StatisticFiles *, QMap<QString, StatisticFiles>);
public slots:
    void slotFindFiles(const QDir&);
};

struct StatisticFiles
{
    quint64 size = 0; //общий размер файлов
    quint64 amount = 0;//количество файлов
    void clear(){
        size = 0;
        amount = 0;
    }
};
#endif // FILEINFO_H
