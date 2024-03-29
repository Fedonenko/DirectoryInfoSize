#include"fileinfo.h"

FileInfo::FileInfo(QDir dir, bool isThread, QObject *parent) : QObject(parent),
    isThread(isThread), dir(dir)
{
    maxPrBar = 1;
    valuePrBar = 0;
    isRun = true;
}
FileInfo::~FileInfo(){
}

void FileInfo::start(const QDir &dir){
    if(!isRun){
        return;
    }
//    if(!isThread){
        QCoreApplication::processEvents();
//    }


    QStringList listFile = dir.entryList(QDir::Files);
    maxPrBar += listFile.size();
    qint64 fileSize;
    QString suffix;
    foreach (QString tmpPath, listFile) {
        QFileInfo fi(dir, tmpPath);
        suffix = fi.suffix();
        emit prBarInf(maxPrBar, ++valuePrBar);
        sizeFiles[suffix].amount++;
        fileSize = fi.size();
        sizeFiles[suffix].size += fileSize;
        StatisticAllFiles.amount++;
        StatisticAllFiles.size += fileSize;
    }
    QStringList listDir = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (auto tmpPath, listDir) {
        start(QDir(dir.absoluteFilePath(tmpPath)));
    }
}
void FileInfo::slotStop(){
    isRun = false;
}
void FileInfo::slotFindFiles(){
    if(!dir.entryList(QDir::Files).isEmpty() or !dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty()){
        start(dir);
        valuePrBar++;
    }
    else{
        maxPrBar = 1;
        valuePrBar = 1;
    }
    emit prBarInf(maxPrBar, valuePrBar);
    emit endFileInfo(StatisticAllFiles, sizeFiles);
}
QString FileInfo::fileSize(quint64 n){
    int i = 0;
    double nSize = n;
    for(; nSize > 1023; nSize /= 1024, i++){
        if(i >= 4){
            break;
        }
    }
    return QString("%1%2").arg(nSize, 0, 'f', 2).arg("BKMGT"[i]);

}
