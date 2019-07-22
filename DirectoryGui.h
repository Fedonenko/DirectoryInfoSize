#ifndef DIRECTORYGUI_H
#define DIRECTORYGUI_H

#include<QtWidgets>
#include"fileinfo.h"
#include<QtConcurrent/QtConcurrent>
Q_DECLARE_METATYPE(StatisticFiles)

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
    ///получает размер в байтах, возвращает в виде более удобночитабельной строки
    QString fileSize(quint64);
    ///заполняет переданную строку таблицы содержимым из карты
    void outInTable(int, QMap<QString, StatisticFiles>&);

private slots:
    ///запускает поиск файлов от корня переданного значения модели(в этом или отдельном потоке)
    void slotFindFiles(const QModelIndex&);
    ///обновляет значение прогресБара, (max, value)
    void slotPrBarUpdate(int, int);
    ///получает статистику и выводит в таблицу
    void slotEndFileInfo(const StatisticFiles StatisticAllFiles, QMap<QString, StatisticFiles>);
signals:

};



#endif // DIRECTORYGUI_H
