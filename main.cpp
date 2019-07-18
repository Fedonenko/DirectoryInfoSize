#include <QApplication>
#include<DirectoryGui.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DirectoryGui obj;
    obj.show();

    return a.exec();
}
