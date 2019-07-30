#ifndef FILEINFO_H
#define FILEINFO_H

#include<QObject>
#include<QApplication>
#include<QString>
#include<QMap>
#include<QDir>

struct StatisticFiles
{
    quint64 size = 0; //общий размер файлов
    quint64 amount = 0;//количество файлов
    void clear(){
        size = 0;
        amount = 0;
    }
};
//struct StatisticFiles;

class FileInfo : public QObject
{
    Q_OBJECT
private:
    ///для прокрутки событий при запуске в том же потоке, что б не замирало окно
    bool isThread;
    bool isRun;
    int maxPrBar;
    int valuePrBar;
    StatisticFiles StatisticAllFiles;
    QMap<QString, StatisticFiles> sizeFiles;
    const QDir dir;


public:
    FileInfo( QDir dir = QApplication::applicationDirPath(), bool isThread = false, QObject *parent = Q_NULLPTR);
    ~FileInfo();
private:
    ///собирает информацию о количестве и размере файлов
    void start(const QDir&);

    QString fileSize(quint64);
signals:
    ///информация для QProgressBar (max, value)
    void prBarInf(int, int);
    ///отправляет статистику по размерам файлов
    void endFileInfo(const StatisticFiles ,const QMap<QString, StatisticFiles>);
public slots:
    ///поиск файлов и сбор статистики по ним
    void slotFindFiles();
    void slotStop();
};

#endif // FILEINFO_H
