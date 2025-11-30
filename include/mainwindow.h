/**
 * @file mainwindow.h
 * @brief 主窗口界面类
 * 
 * 提供两个标签页：
 * - 标签页①: L系列线扫相机转换（固定宽度模式）
 * - 标签页②: A系列面阵相机转换（自定义宽高模式）
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QTabWidget>
#include "ply_processor.h"              // L系列线扫相机处理器
#include "advanced_ply_processor.h"     // A系列面阵相机处理器
#include <vector>
#include <string>

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
    void onAdvancedBrowseButtonClicked();
    void onAdvancedStartConversionClicked();
    void onAdvancedConversionFinished();
    void onAdvancedConversionProgress(int value);
    void appendLog(const QString& message);
    void loadPropertyNames(const QString& folderPath);

private:
    // Log system
    void initLogSystem();
    void writeLogToFile(const QString& message);
    QString getLogFilePath();
    QWidget* createFixedModeTab();
    QWidget* createAdvancedModeTab();
    // UI elements
    QTabWidget* tabWidget;
    QLineEdit* inputFolderEdit;
    QLineEdit* widthEdit;
    QComboBox* grayPropertyComboBox;
    QComboBox* channelOrderComboBox;
    QPushButton* browseButton;
    QPushButton* convertButton;
    QLabel* inputFolderLabel;
    QLabel* widthLabel;
    QLabel* grayPropertyLabel;
    QLabel* channelOrderLabel;
    QProgressBar* progressBar;
    QTextEdit* logTextEdit;
    QLineEdit* advancedInputFolderEdit;
    QLineEdit* advancedWidthEdit;
    QLineEdit* advancedHeightEdit;
    QPushButton* advancedBrowseButton;
    QPushButton* advancedConvertButton;
    QProgressBar* advancedProgressBar;
    
    // Processing thread
    QThread* processingThread;
    PlyProcessor* processor;
    QThread* advancedProcessingThread;
    AdvancedPlyProcessor* advancedProcessor;
    
    QString inputFolderPath;
    QString advancedInputFolderPath;
    
    // Log system
    QString logFolderPath;
    QFile* logFile;
    QTextStream* logStream;
};

#endif // MAINWINDOW_H