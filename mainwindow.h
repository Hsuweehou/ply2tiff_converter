#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QLabel>
#include "ply_tiff_converter_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNavigationItemClicked(QTreeWidgetItem* item, int column);

private:
    void setupUI();
    void setupNavigation();
    
    // UI elements
    QTreeWidget* navigationTree;
    QStackedWidget* contentStack;
    QLabel* contentTitleLabel;
    
    // Pages
    PlyTiffConverterWidget* plyTiffConverterPage;
};

#endif // MAINWINDOW_H
