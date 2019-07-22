#ifndef DIRECTORYGUI_H
#define DIRECTORYGUI_H

#include<QtWidgets>
#include"fileinfo.h"
#include<QtConcurrent/QtConcurrent>
Q_DECLARE_METATYPE(StatisticFiles)
//Q_DECLARE_METATYPE(QMap<QString,StatisticFiles>)

struct StatisticFiles;

class DirectoryGui : public QWidget
{
    Q_OBJECT
private:

    QTreeView *p_treeView;
    QFileSystemModel *p_fileSystemModel;

    QTableWidget *p_tab;
    QProgressBar *p_pBarFile;
    QLabel* p_lblForPBFile;
    QLabel* p_lblWait;

    QMap<QString, StatisticFiles> sizeFiles;
    StatisticFiles *StatisticAllFiles;
    FileInfo *fileInfo;
    QThread *thread;

    QStringList listColumn;
    const QString allFiles = "Всего";
    bool isDoFind = false;
public:
    DirectoryGui(QWidget *pwgt = Q_NULLPTR);
    ~DirectoryGui();
private:
    ///собирает информацию о количестве и размере файлов
    //void start(const QDir&);
    //void startThread(const QDir&);
    ///получает размер в байтах, возвращает в виде более удобночитабельной строки
    QString fileSize(quint64);
    ///заполняет строку переданную строку таблицы содержимым из карты
    //void outInTable(int row);
    void outInTableThread(int, QMap<QString, StatisticFiles>&);

    void findFilesThread(const QDir dir);
private slots:
    ///запускает поиск файлов в от корня QTreeWidgetItem
    //void slotFindFiles(const QModelIndex&);
    void slotFindFilesThread(const QModelIndex&);
    ///обновляет значение прогресБара, (max, value)
    void slotPrBarUpdate(int, int);
    ///получает статистику и выводит в таблицу
    void slotEndFileInfo(const StatisticFiles StatisticAllFiles, QMap<QString, StatisticFiles>);
signals:

};
///структура для ханения информации о файлах с одиннаковым расширение



#endif // DIRECTORYGUI_H
