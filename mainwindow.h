#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QThread>
#include "ply_processor.h"

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
    void onBrowseButtonClicked();
    void onStartConversionClicked();
    void onConversionFinished();
    void onConversionProgress(int value);
    void appendLog(const QString& message);

private:
    // UI elements
    QLineEdit* inputFolderEdit;
    QLineEdit* widthEdit;
    QPushButton* browseButton;
    QPushButton* convertButton;
    QLabel* inputFolderLabel;
    QLabel* widthLabel;
    QProgressBar* progressBar;
    QTextEdit* logTextEdit;
    
    // Processing thread
    QThread* processingThread;
    PlyProcessor* processor;
    
    QString inputFolderPath;
};

#endif // MAINWINDOW_H