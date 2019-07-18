#ifndef DIRECTORYGUI_H
#define DIRECTORYGUI_H

#include<QtWidgets>

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

    QStringList listColumn;
    const QString allFiles = "Всего";
    bool isDoFind = false;
public:
    DirectoryGui(QWidget *pwgt = Q_NULLPTR);
    ~DirectoryGui();
private:
    ///собирает информацию о количестве и размере файлов
    void start(const QDir&);
    ///получает размер в байтах, возвращает в виде более удобночитабельной строки
    QString fileSize(quint64);
    ///заполняет строку переданную строку таблицы содержимым изкарты
    void outInTable(int row);
private slots:
    ///запускает поиск файлов в от корня QTreeWidgetItem
    void slotFindFiles(const QModelIndex&);
signals:

};
///структура для ханения иноформации о файлах с одиннаковым расширение
struct StatisticFiles
{
    quint64 size = 0; //общий размер файлов
    quint64 amount = 0;//количество файлов
    void clear(){
        size = 0;
        amount = 0;
    }
};


#endif // DIRECTORYGUI_H
