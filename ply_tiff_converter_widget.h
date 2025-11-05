#ifndef PLY_TIFF_CONVERTER_WIDGET_H
#define PLY_TIFF_CONVERTER_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include "ply_processor.h"
#include <vector>
#include <string>

class PlyTiffConverterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlyTiffConverterWidget(QWidget *parent = nullptr);
    ~PlyTiffConverterWidget();

private slots:
    void onBrowseButtonClicked();
    void onStartConversionClicked();
    void onConversionFinished();
    void onConversionProgress(int value);
    void appendLog(const QString& message);
    void loadPropertyNames();

private:
    // Log system
    void initLogSystem();
    void writeLogToFile(const QString& message);
    QString getLogFilePath();
    
    // UI elements
    QLineEdit* inputFolderEdit;
    QLineEdit* widthEdit;
    QComboBox* grayPropertyComboBox;
    QPushButton* browseButton;
    QPushButton* convertButton;
    QLabel* inputFolderLabel;
    QLabel* widthLabel;
    QLabel* grayPropertyLabel;
    QProgressBar* progressBar;
    QTextEdit* logTextEdit;
    
    // Processing thread
    QThread* processingThread;
    PlyProcessor* processor;
    
    QString inputFolderPath;
    
    // Log system
    QString logFolderPath;
    QFile* logFile;
    QTextStream* logStream;
};

#endif // PLY_TIFF_CONVERTER_WIDGET_H

