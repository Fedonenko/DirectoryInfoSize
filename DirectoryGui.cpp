#include "DirectoryGui.h"

DirectoryGui::DirectoryGui(QWidget *pwgt) : QWidget(pwgt),
    fileInfo(Q_NULLPTR), thread(Q_NULLPTR)
{
    StatisticAllFiles = new StatisticFiles;

    p_treeView = new QTreeView;
    p_fileSystemModel = new QFileSystemModel;

    p_fileSystemModel->setRootPath(QDir::rootPath());
    p_fileSystemModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    p_treeView->setModel(p_fileSystemModel);
    for(int i = 1; i < p_fileSystemModel->columnCount(); i++){
        p_treeView->hideColumn(i);
    }
    qRegisterMetaType<StatisticFiles>("StatisticFiles");
    qRegisterMetaType<QMap<QString,StatisticFiles>>("QMap<QString,StatisticFiles>");
    connect(p_treeView, SIGNAL(clicked(QModelIndex)),
            this,       SLOT(slotFindFiles(QModelIndex)));

    QWidget *p_rWgt = new QWidget;
    QSplitter *p_splitterHorizontal = new QSplitter(Qt::Horizontal);
    p_splitterHorizontal->addWidget(p_treeView);
    p_splitterHorizontal->addWidget(p_rWgt);


    p_tab = new QTableWidget(1, 3, this);
    listColumn << "Общий размер" << "Средний размер" << "Количество файлов";
    p_tab->setHorizontalHeaderLabels(listColumn);
    QStringList tmpLst;
    p_tab->setVerticalHeaderLabels(tmpLst);

    p_pBarFile = new QProgressBar;
    p_pBarFile->setMaximumSize(150, 10);
    p_lblForPBFile = new QLabel("Готов");
    p_lblWait = new QLabel;

    //set Layout

    QHBoxLayout* p_rHBLayout = new QHBoxLayout;

    p_rHBLayout->addWidget(p_pBarFile);
    p_rHBLayout->addWidget(p_lblForPBFile);
    p_rHBLayout->addWidget(p_lblWait);

    QVBoxLayout *p_rVBLayout = new QVBoxLayout;

    p_rVBLayout->addWidget(p_tab);
    p_rVBLayout->addLayout(p_rHBLayout);

    p_rWgt->setLayout(p_rVBLayout);

    QHBoxLayout *p_hBLayout = new QHBoxLayout;

    p_hBLayout->addWidget(p_splitterHorizontal);
    setLayout(p_hBLayout);

}
DirectoryGui::~DirectoryGui(){
    delete StatisticAllFiles;
    if(fileInfo){
        delete fileInfo;
    }
//    if(thread){
//        thread->quit();
//        thread->wait();
//        delete thread;
//        thread = Q_NULLPTR;
//    }

}
void DirectoryGui::closeEvent(QCloseEvent *){
    if(thread){
        emit stopped();
        thread->quit();
        thread->wait();
        delete thread;
        thread = Q_NULLPTR;
    }

    qDebug() << "Closed widget";
}

QString DirectoryGui::fileSize(quint64 n){
    int i = 0;
    double nSize = n;
    for(; nSize > 1023; nSize /= 1024, i++){
        if(i >= 4){
            break;
        }
    }
    return QString("%1%2").arg(nSize, 0, 'f', 2).arg("BKMGT"[i]);
}
void DirectoryGui::outInTable(int row, QMap<QString, StatisticFiles>&sizeFiles){
    QTableWidgetItem *pTabWgtItem = p_tab->item(row, 0);
    QString mapItem = p_tab->verticalHeaderItem(row)->text();
    if(!sizeFiles.contains(mapItem)){
        qDebug() << "В карте не найдено имя " << mapItem;
        return;
    }
    if(pTabWgtItem != Q_NULLPTR){
        pTabWgtItem->setText(fileSize(sizeFiles[mapItem].size));
    }
    else{
        pTabWgtItem = new QTableWidgetItem(fileSize(sizeFiles[mapItem].size));
        p_tab->setItem(row, 0, pTabWgtItem);
    }
    pTabWgtItem = p_tab->item(row, 1);
    if(pTabWgtItem){
        pTabWgtItem->setText(
                    fileSize(
                        (sizeFiles[mapItem].amount != 0)?
                            sizeFiles[mapItem].size/sizeFiles[mapItem].amount : 0
                                      ));
    }
    else{
        pTabWgtItem = new QTableWidgetItem(fileSize((sizeFiles[mapItem].amount != 0)?
                                                        sizeFiles[mapItem].size/sizeFiles[mapItem].amount : 0));
        p_tab->setItem(row, 1, pTabWgtItem);
    }
    pTabWgtItem = p_tab->item(row, 2);
    if(pTabWgtItem){
        pTabWgtItem->setText(QString::number(sizeFiles[mapItem].amount));
    }
    else{
        pTabWgtItem = new QTableWidgetItem(QString::number(sizeFiles[mapItem].amount));
        p_tab->setItem(row, 2, pTabWgtItem);
    }

}
//______________SLOTS
void DirectoryGui::slotFindFiles(const QModelIndex &modelIndex){
    if(isDoFind){
        p_lblWait->setText("Подождите!");
        return;
    }
    isDoFind = true;

    sizeFiles.clear();
    StatisticAllFiles->clear();
    p_pBarFile->reset();
    p_pBarFile->setMaximum(0);
    p_lblForPBFile->setText("Поиск файлов");

    QDir dir(p_fileSystemModel->filePath(modelIndex));
//___________условие выбора подсчёта статистики в текущем потоке или в отдельном
    fileInfo = (QThread::idealThreadCount() > 1)? new FileInfo(dir, true) : new FileInfo(dir);

    connect(fileInfo, SIGNAL(prBarInf(int,int)),
            this,     SLOT(slotPrBarUpdate(int,int)));
    connect(fileInfo, SIGNAL(endFileInfo(StatisticFiles, QMap<QString,StatisticFiles>)),
            this,     SLOT(slotEndFileInfo(StatisticFiles, QMap<QString,StatisticFiles>)));

    if(QThread::idealThreadCount() > 1){
        thread = new QThread;
        fileInfo->moveToThread(thread);
        connect(this, SIGNAL(stopped()),
                fileInfo, SLOT(slotStop()));
        connect(thread,  SIGNAL(started()),
                fileInfo, SLOT(slotFindFiles()));
        thread->start();
    }
    else{
        fileInfo->slotFindFiles();
    }
}
void DirectoryGui::slotPrBarUpdate(int max, int value){
    p_pBarFile->setMaximum(max);
    p_pBarFile->setValue(value);
}
void DirectoryGui::slotEndFileInfo(const StatisticFiles StatisticAllFiles, QMap<QString, StatisticFiles> sizeFiles){
    p_lblForPBFile->setText("Построение таблицы вывода");
    int countFiles = sizeFiles.size();
    QStringList listHeaders = sizeFiles.keys();

    p_tab->clear();
    p_tab->setRowCount(countFiles);
    p_tab->setColumnCount(listColumn.size());

    p_tab->setHorizontalHeaderLabels(listColumn);
    if(!listHeaders.isEmpty()){
        p_tab->setVerticalHeaderLabels(listHeaders);
    }

    for(auto i = 0; i < p_tab->rowCount(); i++){
        outInTable(i, sizeFiles);
    }

    p_tab->insertRow(p_tab->rowCount());
    p_tab->setItem(p_tab->rowCount()-1, 0, new QTableWidgetItem(fileSize(StatisticAllFiles.size)));
    p_tab->setItem(p_tab->rowCount()-1, 1, new QTableWidgetItem(fileSize((StatisticAllFiles.amount > 0)?StatisticAllFiles.size/StatisticAllFiles.amount:0)));
    p_tab->setItem(p_tab->rowCount()-1, 2, new QTableWidgetItem(QString::number(StatisticAllFiles.amount)));
    p_tab->setVerticalHeaderItem(p_tab->rowCount()-1, new QTableWidgetItem("Всего"));

    p_lblWait->clear();
    delete fileInfo;
    fileInfo = Q_NULLPTR;
    if(thread){
        thread->quit();
        thread->wait();
        delete thread;
        thread = Q_NULLPTR;
    }

    p_lblForPBFile->setText("Готов");

    isDoFind = false;
}


