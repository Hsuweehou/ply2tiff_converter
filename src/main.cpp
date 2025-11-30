#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // 应用程序图标（可选，全局设置）
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/app.ico"), QSize(), QIcon::Normal, QIcon::Off);
    app.setWindowIcon(icon);
    MainWindow window;
    window.show();
    
    return app.exec();
}