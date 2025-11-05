#include "ply_tiff_converter_widget.h"
#include "ui_strings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGroupBox>
#include <QApplication>
#include <QDateTime>
#include <QCoreApplication>

PlyTiffConverterWidget::PlyTiffConverterWidget(QWidget *parent)
    : QWidget(parent)
    , processingThread(nullptr)
    , processor(nullptr)
    , logFile(nullptr)
    , logStream(nullptr)
{
    // 初始化日志系统
    initLogSystem();
    
    // 设置整体背景
    setStyleSheet("background-color: transparent;");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    // 输入设置区域 - 卡片式设计
    QGroupBox* settingsGroup = new QGroupBox(UIStrings::conversionSettings(), this);
    settingsGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 15px;"
        "    font-weight: bold;"
        "    border: none;"
        "    border-radius: 10px;"
        "    margin-top: 12px;"
        "    padding-top: 20px;"
        "    background-color: #ffffff;"
        "    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 20px;"
        "    padding: 0 8px;"
        "    color: #1e3c72;"
        "}"
    );
    
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setContentsMargins(15, 20, 15, 15);
    settingsLayout->setSpacing(12);
    
    // 输入文件夹选择
    QHBoxLayout* folderLayout = new QHBoxLayout();
    folderLayout->setSpacing(10);
    inputFolderLabel = new QLabel(this);
    inputFolderLabel->setText(UIStrings::inputFolder());
    inputFolderLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    inputFolderLabel->setMinimumWidth(110);
    
    inputFolderEdit = new QLineEdit(this);
    inputFolderEdit->setReadOnly(true);
    inputFolderEdit->setPlaceholderText(UIStrings::placeholderText());
    inputFolderEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px 15px;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 6px;"
        "    background-color: #f8f9fa;"
        "    font-size: 13px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "    background-color: #ffffff;"
        "}"
    );
    
    browseButton = new QPushButton(UIStrings::browse(), this);
    browseButton->setFixedWidth(110);
    browseButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #3498db, stop:1 #2980b9);"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 6px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #2980b9, stop:1 #21618c);"
        "}"
        "QPushButton:pressed {"
        "    background: #21618c;"
        "}"
    );
    
    folderLayout->addWidget(inputFolderLabel);
    folderLayout->addWidget(inputFolderEdit);
    folderLayout->addWidget(browseButton);
    
    // 宽度设置 + 灰度属性同一行
    QHBoxLayout* widthLayout = new QHBoxLayout();
    widthLayout->setSpacing(10);
    
    widthLabel = new QLabel(this);
    widthLabel->setText(UIStrings::imageWidth());
    widthLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    widthLabel->setMinimumWidth(110);
    
    widthEdit = new QLineEdit(this);
    widthEdit->setText("3200");
    widthEdit->setFixedWidth(120);
    widthEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px 15px;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 6px;"
        "    background-color: white;"
        "    font-size: 13px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
    );
    
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(widthEdit);

    // 灰度属性名称（与图像宽度同一行）
    widthLayout->addSpacing(30);
    grayPropertyLabel = new QLabel(this);
    grayPropertyLabel->setText(UIStrings::grayProperty());
    grayPropertyLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    
    grayPropertyComboBox = new QComboBox(this);
    grayPropertyComboBox->setFixedWidth(180);
    grayPropertyComboBox->setEditable(false);
    grayPropertyComboBox->setStyleSheet(
        "QComboBox {"
        "    padding: 10px 15px;"
        "    padding-right: 35px;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 6px;"
        "    background-color: white;"
        "    font-size: 13px;"
        "    color: #2c3e50;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 35px;"
        "    border-left: 1px solid #e0e0e0;"
        "    background-color: #f8f9fa;"
        "    border-top-right-radius: 4px;"
        "    border-bottom-right-radius: 4px;"
        "}"
        "QComboBox::drop-down:hover {"
        "    background-color: #e9ecef;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    width: 0;"
        "    height: 0;"
        "    border-left: 6px solid transparent;"
        "    border-right: 6px solid transparent;"
        "    border-top: 8px solid #495057;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 2px solid #3498db;"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "    padding: 5px;"
        "}"
    );

    widthLayout->addWidget(grayPropertyLabel);
    widthLayout->addWidget(grayPropertyComboBox);
    widthLayout->addStretch();
    
    settingsLayout->addLayout(folderLayout);
    settingsLayout->addLayout(widthLayout);
    
    // 开始转换按钮
    convertButton = new QPushButton(UIStrings::startConversion(), this);
    convertButton->setEnabled(false);
    convertButton->setMinimumHeight(50);
    convertButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #27ae60, stop:1 #2ecc71);"
        "    color: white;"
        "    border: none;"
        "    padding: 14px;"
        "    border-radius: 8px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #229954, stop:1 #27ae60);"
        "}"
        "QPushButton:pressed {"
        "    background: #1e8449;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "    color: #ecf0f1;"
        "}"
    );
    
    // 进度条
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setMinimumHeight(35);
    progressBar->setTextVisible(true);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: none;"
        "    border-radius: 10px;"
        "    text-align: center;"
        "    background-color: #e9ecef;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        "QProgressBar::chunk {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border-radius: 10px;"
        "}"
    );
    
    // 日志区域 - 卡片式设计
    QWidget* logCard = new QWidget(this);
    logCard->setStyleSheet(
        "QWidget {"
        "    background-color: #ffffff;"
        "    border-radius: 10px;"
        "}"
    );
    QVBoxLayout* logCardLayout = new QVBoxLayout(logCard);
    logCardLayout->setContentsMargins(20, 15, 20, 20);
    logCardLayout->setSpacing(10);
    
    // 日志标题栏
    QWidget* logHeaderWidget = new QWidget(logCard);
    logHeaderWidget->setStyleSheet("background: transparent;");
    QHBoxLayout* logHeaderLayout = new QHBoxLayout(logHeaderWidget);
    logHeaderLayout->setContentsMargins(0, 0, 0, 0);
    logHeaderLayout->setSpacing(10);
    
    QLabel* logLabel = new QLabel(logHeaderWidget);
    logLabel->setText(UIStrings::conversionLog());
    logLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #1e3c72;");
    
    QPushButton* clearLogButton = new QPushButton(QString::fromUtf8("\xe6\xb8\x85\xe9\x99\xa4\xe6\x97\xa5\xe5\xbf\x97"), logHeaderWidget); // 清除日志
    clearLogButton->setFixedWidth(90);
    clearLogButton->setFixedHeight(30);
    clearLogButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 5px 12px;"
        "    border-radius: 5px;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a93226;"
        "}"
    );
    connect(clearLogButton, &QPushButton::clicked, [this]() {
        logTextEdit->clear();
        appendLog(UIStrings::welcome());
        appendLog(UIStrings::info1());
        appendLog(UIStrings::info2());
        appendLog(UIStrings::info3());
    });
    
    logHeaderLayout->addWidget(logLabel);
    logHeaderLayout->addStretch();
    logHeaderLayout->addWidget(clearLogButton);
    
    // 日志输出
    logTextEdit = new QTextEdit(logCard);
    logTextEdit->setReadOnly(true);
    logTextEdit->setStyleSheet(
        "QTextEdit {"
        "    border: 1px solid #dee2e6;"
        "    border-radius: 8px;"
        "    padding: 12px;"
        "    background-color: #1a1d23;"
        "    color: #e9ecef;"
        "    font-family: 'Consolas', 'Courier New', monospace;"
        "    font-size: 12px;"
        "    line-height: 1.5;"
        "}"
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #2d3139;"
        "    width: 10px;"
        "    border-radius: 5px;"
        "    margin: 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    min-height: 30px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #7b8ef0, stop:1 #8b5cb8);"
        "}"
        "QScrollBar::handle:vertical:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #5568d3, stop:1 #6a3e8f);"
        "}"
        "QScrollBar::add-line:vertical {"
        "    border: none;"
        "    background: none;"
        "    height: 0px;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: none;"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
        "QScrollBar:horizontal {"
        "    border: none;"
        "    background: #34495e;"
        "    height: 12px;"
        "    border-radius: 6px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3498db, stop:1 #2ecc71);"
        "    min-width: 30px;"
        "    border-radius: 6px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5dade2, stop:1 #52be80);"
        "}"
        "QScrollBar::handle:horizontal:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2980b9, stop:1 #27ae60);"
        "}"
        "QScrollBar::add-line:horizontal {"
        "    border: none;"
        "    background: none;"
        "    width: 0px;"
        "}"
        "QScrollBar::sub-line:horizontal {"
        "    border: none;"
        "    background: none;"
        "    width: 0px;"
        "}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
        "    background: none;"
        "}"
    );
    
    // 将日志文本框添加到日志卡片
    logCardLayout->addWidget(logHeaderWidget);
    logCardLayout->addWidget(logTextEdit, 1);
    
    // 添加到主布局
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(convertButton);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(logCard, 1);
    
    // 连接信号和槽
    connect(browseButton, &QPushButton::clicked, this, &PlyTiffConverterWidget::onBrowseButtonClicked);
    connect(convertButton, &QPushButton::clicked, this, &PlyTiffConverterWidget::onStartConversionClicked);
    
    // 初始化日志
    appendLog(UIStrings::welcome());
    appendLog(UIStrings::info1());
    appendLog(UIStrings::info2());
    appendLog(UIStrings::info3());
}

PlyTiffConverterWidget::~PlyTiffConverterWidget()
{
    if (processingThread) {
        processingThread->quit();
        processingThread->wait();
        delete processingThread;
    }
    if (processor) {
        delete processor;
    }
    
    // 清理日志系统
    if (logStream) {
        delete logStream;
        logStream = nullptr;
    }
    if (logFile) {
        if (logFile->isOpen()) {
            logFile->close();
        }
        delete logFile;
        logFile = nullptr;
    }
}

void PlyTiffConverterWidget::onBrowseButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, UIStrings::selectFolder(), QDir::homePath());
    if (!folderPath.isEmpty()) {
        inputFolderPath = folderPath;
        inputFolderEdit->setText(folderPath);
        convertButton->setEnabled(true);
        appendLog(UIStrings::selectedFolder() + folderPath);
        loadPropertyNames();
    }
}

void PlyTiffConverterWidget::onStartConversionClicked()
{
    if (inputFolderPath.isEmpty()) {
        QMessageBox::warning(this, UIStrings::warning(), UIStrings::pleaseSelectFolder());
        return;
    }
    
    bool ok;
    int width = widthEdit->text().toInt(&ok);
    if (!ok || width <= 0 || width > 3200) {
        QMessageBox::warning(this, UIStrings::warning(), UIStrings::validWidth());
        return;
    }
    
    QString grayPropertyName = grayPropertyComboBox->currentText();
    if (grayPropertyName.isEmpty()) {
        QMessageBox::warning(this, UIStrings::warning(), UIStrings::selectProperty());
        return;
    }
    
    // 禁用UI元素
    browseButton->setEnabled(false);
    convertButton->setEnabled(false);
    widthEdit->setEnabled(false);
    grayPropertyComboBox->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // 创建处理器对象和线程
    processingThread = new QThread(this);
    processor = new PlyProcessor();
    processor->moveToThread(processingThread);
    
    // 连接信号和槽
    connect(processingThread, &QThread::started, [=]() {
        processor->process(inputFolderPath.toStdString(), width, grayPropertyName.toStdString());
    });
    
    connect(processor, &PlyProcessor::progressUpdated, this, &PlyTiffConverterWidget::onConversionProgress);
    connect(processor, &PlyProcessor::finished, this, &PlyTiffConverterWidget::onConversionFinished);
    connect(processor, &PlyProcessor::logMessage, this, &PlyTiffConverterWidget::appendLog);
    
    // 开始处理
    appendLog(UIStrings::startProcess());
    processingThread->start();
}

void PlyTiffConverterWidget::onConversionFinished()
{
    if (processingThread && processingThread->isRunning()) {
        processingThread->quit();
        processingThread->wait();
    }
    
    // 恢复UI元素
    browseButton->setEnabled(true);
    convertButton->setEnabled(true);
    widthEdit->setEnabled(true);
    grayPropertyComboBox->setEnabled(true);
    progressBar->setVisible(false);
    
    appendLog(UIStrings::complete());
    QMessageBox::information(this, UIStrings::completed(), UIStrings::conversionCompleted());
}

void PlyTiffConverterWidget::onConversionProgress(int value)
{
    progressBar->setValue(value);
}

void PlyTiffConverterWidget::appendLog(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString coloredMessage;
    
    // 根据消息内容设置不同的颜色
    if (message.contains(UIStrings::kwWelcome()) || message.contains(UIStrings::kwComplete())) {
        coloredMessage = QString("<span style='color: #2ecc71;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(UIStrings::kwWarning()) || message.contains(UIStrings::kwError()) || message.contains(UIStrings::kwFailed())) {
        coloredMessage = QString("<span style='color: #e74c3c;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(UIStrings::kwStart()) || message.contains(UIStrings::kwProcessing())) {
        coloredMessage = QString("<span style='color: #3498db;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(UIStrings::kwLoaded()) || message.contains(UIStrings::kwSelected())) {
        coloredMessage = QString("<span style='color: #9b59b6;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else {
        coloredMessage = QString("<span style='color: #ecf0f1;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    }
    
    logTextEdit->append(coloredMessage);
    
    // 将日志写入文件
    writeLogToFile(message);
}

void PlyTiffConverterWidget::loadPropertyNames()
{
    if (inputFolderPath.isEmpty()) {
        return;
    }
    
    // 清空下拉框
    grayPropertyComboBox->clear();
    
    // 递归查找PLY文件
    QStringList filters;
    filters << "*.ply";
    QDirIterator it(inputFolderPath, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    QStringList filePaths;
    while (it.hasNext()) {
        filePaths << it.next();
    }
    
    if (filePaths.isEmpty()) {
        appendLog(UIStrings::noFileFound());
        return;
    }
    
    // 读取第一个PLY文件的属性名称
    PlyProcessor tempProcessor;
    QString firstPlyFile = filePaths.first();
    std::vector<std::string> properties = tempProcessor.getPropertyNames(firstPlyFile.toStdString());
    
    if (properties.empty()) {
        appendLog(UIStrings::cannotReadFile());
        return;
    }
    
    // 填充下拉框
    for (const std::string& prop : properties) {
        grayPropertyComboBox->addItem(QString::fromStdString(prop));
    }
    
    // 如果有"gray"属性，默认选中它，否则选中第一个
    int grayIndex = grayPropertyComboBox->findText("gray");
    if (grayIndex >= 0) {
        grayPropertyComboBox->setCurrentIndex(grayIndex);
    } else {
        grayPropertyComboBox->setCurrentIndex(0);
    }
    
    appendLog(UIStrings::loadedProperties() + QString::number(properties.size()) + UIStrings::properties() + QFileInfo(firstPlyFile).fileName());
}

// 初始化日志系统
void PlyTiffConverterWidget::initLogSystem()
{
    // 获取程序运行目录
    QString appDirPath = QCoreApplication::applicationDirPath();
    logFolderPath = appDirPath + "/log";
    
    // 创建log文件夹
    QDir dir;
    if (!dir.exists(logFolderPath)) {
        if (!dir.mkpath(logFolderPath)) {
            return;
        }
    }
    
    // 获取当前日期的日志文件路径
    QString logFilePath = getLogFilePath();
    
    // 打开日志文件
    logFile = new QFile(logFilePath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        logStream = new QTextStream(logFile);
        logStream->setCodec("UTF-8");
        
        *logStream << "\n";
        *logStream << "========================================\n";
        *logStream << UIStrings::programStartTime() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        *logStream << "========================================\n";
        logStream->flush();
    }
}

// 获取当前日期的日志文件路径
QString PlyTiffConverterWidget::getLogFilePath()
{
    QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
    return logFolderPath + "/" + currentDate + ".log";
}

// 写入日志到文件
void PlyTiffConverterWidget::writeLogToFile(const QString& message)
{
    if (!logStream || !logFile || !logFile->isOpen()) {
        return;
    }
    
    // 检查日期是否变化，如果变化则切换到新的日志文件
    QString currentLogFile = getLogFilePath();
    if (currentLogFile != logFile->fileName()) {
        logStream->flush();
        delete logStream;
        logFile->close();
        delete logFile;
        
        logFile = new QFile(currentLogFile);
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            logStream = new QTextStream(logFile);
            logStream->setCodec("UTF-8");
            
            *logStream << "\n";
            *logStream << "========================================\n";
            *logStream << UIStrings::newDateSession() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
            *logStream << "========================================\n";
        } else {
            logStream = nullptr;
            return;
        }
    }
    
    // 写入日志
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    *logStream << "[" << timestamp << "] " << message << "\n";
    logStream->flush();
}
