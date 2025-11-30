#include "mainwindow.h"
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
#include <QIcon>
#include <QStandardPaths>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , processingThread(nullptr)
    , processor(nullptr)
    , advancedProcessingThread(nullptr)
    , advancedProcessor(nullptr)
    , logFile(nullptr)
    , logStream(nullptr)
{
    initLogSystem();
    setWindowTitle(QString::fromUtf8("PLY-TIFF 转换器"));
    resize(900, 600);
    setMinimumSize(750, 520);
    setWindowIcon(QIcon(":/icons/app.ico"));

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "    border: 2px solid #3498db;"
        "    border-radius: 10px;"
        "    background: #fdfefe;"
        "    margin-top: 10px;"
        "}"
        "QTabWidget::tab-bar {"
        "    left: 10px;"
        "}"
        "QTabBar::tab {"
        "    background: #ecf0f1;"
        "    color: #34495e;"
        "    padding: 10px 32px;"
        "    min-width: 180px;"
        "    margin-right: 4px;"
        "    margin-left: 4px;"
        "    border-top-left-radius: 8px;"
        "    border-top-right-radius: 8px;"
        "    font-weight: bold;"
        "}"
        "QTabBar::tab:hover {"
        "    background: #d6eaf8;"
        "}"
        "QTabBar::tab:selected {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                stop:0 #5dade2, stop:1 #3498db);"
        "    color: white;"
        "}"
        "QTabBar::tab:selected:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                stop:0 #85c1e9, stop:1 #2e86c1);"
        "}"
    );
    tabWidget->addTab(createFixedModeTab(), QString::fromUtf8("L系列线扫相机"));
    tabWidget->addTab(createAdvancedModeTab(), QString::fromUtf8("A系列面阵相机"));
    mainLayout->addWidget(tabWidget);

    QLabel* logLabel = new QLabel(this);
    logLabel->setText(QString::fromUtf8("<span style='color: #27ae60; font-size: 15px;'>■</span> 转换日志:"));
    logLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e; margin-top: 5px;");

    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setStyleSheet(
        "QTextEdit {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "    background-color: #2c3e50;"
        "    color: #ecf0f1;"
        "    font-family: 'Consolas', 'Courier New', monospace;"
        "    font-size: 11px;"
        "    line-height: 1.4;"
        "}"
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #34495e;"
        "    width: 12px;"
        "    border-radius: 6px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2ecc71);"
        "    min-height: 30px;"
        "    border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5dade2, stop:1 #52be80);"
        "}"
        "QScrollBar::handle:vertical:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2980b9, stop:1 #27ae60);"
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

    mainLayout->addWidget(logLabel);
    mainLayout->addWidget(logTextEdit, 1);

    centralWidget->setStyleSheet("QWidget { background-color: #ffffff; }");

    connect(browseButton, &QPushButton::clicked, this, &MainWindow::onBrowseButtonClicked);
    connect(convertButton, &QPushButton::clicked, this, &MainWindow::onStartConversionClicked);
    connect(advancedBrowseButton, &QPushButton::clicked, this, &MainWindow::onAdvancedBrowseButtonClicked);
    connect(advancedConvertButton, &QPushButton::clicked, this, &MainWindow::onAdvancedStartConversionClicked);

    appendLog(QString::fromUtf8("欢迎使用PLY到TIFF转换器!"));
    appendLog(QString::fromUtf8("标签页①支持固定宽度及灰度属性配置。"));
    appendLog(QString::fromUtf8("标签页②支持XYZ/灰度同时导出(CV_32FC3 + CV_8UC3)。"));
    appendLog(QString::fromUtf8("请选择合适的标签页完成任务。"));
}

QWidget* MainWindow::createFixedModeTab()
{
    QWidget* tab = new QWidget(this);
    QVBoxLayout* tabLayout = new QVBoxLayout(tab);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(12);

    QGroupBox* settingsGroup = new QGroupBox(QString::fromUtf8("■ 转换设置"), tab);
    settingsGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: #f8f9fa;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 15px;"
        "    padding: 0 5px;"
        "    color: #2c3e50;"
        "}"
    );

    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setContentsMargins(15, 20, 15, 15);
    settingsLayout->setSpacing(12);

    QHBoxLayout* folderLayout = new QHBoxLayout();
    folderLayout->setSpacing(10);
    inputFolderLabel = new QLabel(tab);
    inputFolderLabel->setText(QString::fromUtf8("<span style='color: #3498db; font-size: 15px;'>■</span> 输入文件夹:"));
    inputFolderLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    inputFolderLabel->setMinimumWidth(110);

    inputFolderEdit = new QLineEdit(tab);
    inputFolderEdit->setReadOnly(true);
    inputFolderEdit->setPlaceholderText(QString::fromUtf8("请选择包含PLY文件的文件夹..."));
    inputFolderEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px 12px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: #ecf0f1;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
    );

    browseButton = new QPushButton(QString::fromUtf8("► 浏览"), tab);
    browseButton->setFixedWidth(100);
    browseButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 15px;"
        "    border-radius: 5px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );

    folderLayout->addWidget(inputFolderLabel);
    folderLayout->addWidget(inputFolderEdit);
    folderLayout->addWidget(browseButton);

    QHBoxLayout* widthLayout = new QHBoxLayout();
    widthLayout->setSpacing(10);

    widthLabel = new QLabel(tab);
    widthLabel->setText(QString::fromUtf8("<span style='color: #e67e22; font-size: 15px;'>■</span> 图像宽度:"));
    widthLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    widthLabel->setMinimumWidth(110);

    widthEdit = new QLineEdit(tab);
    widthEdit->setText("3200");
    widthEdit->setFixedWidth(100);
    widthEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px 12px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
    );

    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(widthEdit);

    widthLayout->addSpacing(30);
    grayPropertyLabel = new QLabel(tab);
    grayPropertyLabel->setText(QString::fromUtf8("<span style='color: #9b59b6; font-size: 15px;'>■</span> 灰度属性:"));
    grayPropertyLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");

    grayPropertyComboBox = new QComboBox(tab);
    grayPropertyComboBox->setFixedWidth(150);
    grayPropertyComboBox->setEditable(false);
    grayPropertyComboBox->setStyleSheet(
        "QComboBox {"
        "    padding: 8px 12px;"
        "    padding-right: 30px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 30px;"
        "    border-left: 1px solid #bdc3c7;"
        "    background-color: #ecf0f1;"
        "    border-top-right-radius: 3px;"
        "    border-bottom-right-radius: 3px;"
        "}"
        "QComboBox::drop-down:hover {"
        "    background-color: #d5dbdb;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    width: 0;"
        "    height: 0;"
        "    border-left: 6px solid transparent;"
        "    border-right: 6px solid transparent;"
        "    border-top: 8px solid #2c3e50;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 2px solid #3498db;"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
    );

    widthLayout->addWidget(grayPropertyLabel);
    widthLayout->addWidget(grayPropertyComboBox);
    widthLayout->addStretch();

    QHBoxLayout* channelOrderLayout = new QHBoxLayout();
    channelOrderLayout->setSpacing(10);

    channelOrderLabel = new QLabel(tab);
    channelOrderLabel->setText(QString::fromUtf8("<span style='color: #e67e22; font-size: 15px;'>■</span> 通道顺序:"));
    channelOrderLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    channelOrderLabel->setMinimumWidth(110);

    channelOrderComboBox = new QComboBox(tab);
    channelOrderComboBox->setFixedWidth(250);
    channelOrderComboBox->setEditable(false);
    channelOrderComboBox->addItem(QString::fromUtf8("OpenCV格式 (ZYX顺序)"), 0);
    channelOrderComboBox->addItem(QString::fromUtf8("Open3D格式 (XYZ顺序)"), 1);
    channelOrderComboBox->setCurrentIndex(0);
    channelOrderComboBox->setStyleSheet(
        "QComboBox {"
        "    padding: 8px 12px;"
        "    padding-right: 30px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 30px;"
        "    border-left: 1px solid #bdc3c7;"
        "    background-color: #ecf0f1;"
        "    border-top-right-radius: 3px;"
        "    border-bottom-right-radius: 3px;"
        "}"
        "QComboBox::drop-down:hover {"
        "    background-color: #d5dbdb;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    width: 0;"
        "    height: 0;"
        "    border-left: 6px solid transparent;"
        "    border-right: 6px solid transparent;"
        "    border-top: 8px solid #2c3e50;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 2px solid #3498db;"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
    );

    channelOrderLayout->addWidget(channelOrderLabel);
    channelOrderLayout->addWidget(channelOrderComboBox);
    channelOrderLayout->addStretch();

    settingsLayout->addLayout(folderLayout);
    settingsLayout->addLayout(widthLayout);
    settingsLayout->addLayout(channelOrderLayout);

    convertButton = new QPushButton(QString::fromUtf8("▶ 开始转换"), tab);
    convertButton->setEnabled(false);
    convertButton->setMinimumHeight(45);
    convertButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #27ae60;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 8px;"
        "    font-size: 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e8449;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #95a5a6;"
        "    color: #ecf0f1;"
        "}"
    );

    progressBar = new QProgressBar(tab);
    progressBar->setVisible(false);
    progressBar->setMinimumHeight(25);
    progressBar->setTextVisible(true);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    text-align: center;"
        "    background-color: #ecf0f1;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2ecc71);"
        "    border-radius: 6px;"
        "}"
    );

    tabLayout->addWidget(settingsGroup);
    tabLayout->addWidget(convertButton);
    tabLayout->addWidget(progressBar);
    tabLayout->addStretch();

    return tab;
}

QWidget* MainWindow::createAdvancedModeTab()
{
    QWidget* tab = new QWidget(this);
    QVBoxLayout* tabLayout = new QVBoxLayout(tab);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(12);

    QGroupBox* settingsGroup = new QGroupBox(QString::fromUtf8("■ 转换设置"), tab);
    settingsGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: #f8f9fa;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 15px;"
        "    padding: 0 5px;"
        "    color: #2c3e50;"
        "}"
    );

    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setContentsMargins(15, 20, 15, 15);
    settingsLayout->setSpacing(12);

    QHBoxLayout* folderLayout = new QHBoxLayout();
    folderLayout->setSpacing(10);

    QLabel* folderLabel = new QLabel(tab);
    folderLabel->setText(QString::fromUtf8("<span style='color: #3498db; font-size: 15px;'>■</span> 输入文件夹:"));
    folderLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    folderLabel->setMinimumWidth(110);

    advancedInputFolderEdit = new QLineEdit(tab);
    advancedInputFolderEdit->setReadOnly(true);
    advancedInputFolderEdit->setPlaceholderText(QString::fromUtf8("请选择包含PLY文件的文件夹..."));
    advancedInputFolderEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px 12px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: #ecf0f1;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
    );

    advancedBrowseButton = new QPushButton(QString::fromUtf8("► 浏览"), tab);
    advancedBrowseButton->setFixedWidth(100);
    advancedBrowseButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 15px;"
        "    border-radius: 5px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );

    folderLayout->addWidget(folderLabel);
    folderLayout->addWidget(advancedInputFolderEdit);
    folderLayout->addWidget(advancedBrowseButton);

    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->setSpacing(10);

    QLabel* widthLabel = new QLabel(tab);
    widthLabel->setText(QString::fromUtf8("<span style='color: #e67e22; font-size: 15px;'>■</span> 图像宽度:"));
    widthLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");
    widthLabel->setMinimumWidth(110);

    advancedWidthEdit = new QLineEdit(tab);
    advancedWidthEdit->setText("2448");
    advancedWidthEdit->setFixedWidth(100);
    advancedWidthEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px 12px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
    );

    QLabel* heightLabel = new QLabel(tab);
    heightLabel->setText(QString::fromUtf8("<span style='color: #9b59b6; font-size: 15px;'>■</span> 图像高度:"));
    heightLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #34495e;");

    advancedHeightEdit = new QLineEdit(tab);
    advancedHeightEdit->setText("2048");
    advancedHeightEdit->setFixedWidth(100);
    advancedHeightEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px 12px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
    );

    sizeLayout->addWidget(widthLabel);
    sizeLayout->addWidget(advancedWidthEdit);
    sizeLayout->addSpacing(30);
    sizeLayout->addWidget(heightLabel);
    sizeLayout->addWidget(advancedHeightEdit);
    sizeLayout->addStretch();

    QLabel* descLabel = new QLabel(tab);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #7f8c8d; font-size: 11px;");
    descLabel->setText(QString::fromUtf8("说明: 自动读取属性 x/y/z 与 red/green/blue，输出 CV_32FC3 的 xyz TIFF 与三通道 CV_8UC3 的灰度 TIFF（各通道值一致）。"));

    settingsLayout->addLayout(folderLayout);
    settingsLayout->addLayout(sizeLayout);
    settingsLayout->addWidget(descLabel);

    advancedConvertButton = new QPushButton(QString::fromUtf8("▶ 开始转换"), tab);
    advancedConvertButton->setEnabled(false);
    advancedConvertButton->setMinimumHeight(45);
    advancedConvertButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #27ae60;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 8px;"
        "    font-size: 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e8449;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #95a5a6;"
        "    color: #ecf0f1;"
        "}"
    );

    advancedProgressBar = new QProgressBar(tab);
    advancedProgressBar->setVisible(false);
    advancedProgressBar->setMinimumHeight(25);
    advancedProgressBar->setTextVisible(true);
    advancedProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    text-align: center;"
        "    background-color: #ecf0f1;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2ecc71);"
        "    border-radius: 6px;"
        "}"
    );

    tabLayout->addWidget(settingsGroup);
    tabLayout->addWidget(advancedConvertButton);
    tabLayout->addWidget(advancedProgressBar);
    tabLayout->addStretch();

    return tab;
}

MainWindow::~MainWindow()
{
    if (processingThread) {
        processingThread->quit();
        processingThread->wait();
        delete processingThread;
        processingThread = nullptr;
    }
    if (processor) {
        delete processor;
        processor = nullptr;
    }

    if (advancedProcessingThread) {
        advancedProcessingThread->quit();
        advancedProcessingThread->wait();
        delete advancedProcessingThread;
        advancedProcessingThread = nullptr;
    }
    if (advancedProcessor) {
        delete advancedProcessor;
        advancedProcessor = nullptr;
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

void MainWindow::onBrowseButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, QString::fromUtf8("选择包含PLY文件的文件夹"), QDir::homePath());
    if (!folderPath.isEmpty()) {
        inputFolderPath = folderPath;
        inputFolderEdit->setText(folderPath);
        convertButton->setEnabled(true);
        appendLog(QString::fromUtf8("已选择文件夹: ") + folderPath);
        // 加载属性名称
        loadPropertyNames(folderPath);
    }
}

void MainWindow::onStartConversionClicked()
{
    if (inputFolderPath.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择输入文件夹!"));
        return;
    }
    
    bool ok;
    int width = widthEdit->text().toInt(&ok);
    if (!ok || width <= 0 || width > 3200 || typeid(width) != typeid(int)) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入有效的图像宽度!"));
        return;
    }
    
    QString grayPropertyName = grayPropertyComboBox->currentText();
    if (grayPropertyName.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择灰度属性名称!"));
        return;
    }
    
    // 获取通道顺序选择
    int channelOrder = channelOrderComboBox->currentData().toInt();
    
    // 禁用UI元素
    browseButton->setEnabled(false);
    convertButton->setEnabled(false);
    widthEdit->setEnabled(false);
    grayPropertyComboBox->setEnabled(false);
    channelOrderComboBox->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // 创建处理器对象和线程
    processingThread = new QThread(this);
    processor = new PlyProcessor();
    processor->moveToThread(processingThread);
    
    // 连接信号和槽
    connect(processingThread, &QThread::started, [=]() {
        processor->process(inputFolderPath.toStdString(), width, grayPropertyName.toStdString(), channelOrder);
    });
    
    connect(processor, &PlyProcessor::progressUpdated, this, &MainWindow::onConversionProgress);
    connect(processor, &PlyProcessor::finished, this, &MainWindow::onConversionFinished);
    connect(processor, &PlyProcessor::logMessage, this, &MainWindow::appendLog);
    
    // 开始处理
    appendLog(QString::fromUtf8("开始转换过程..."));
    processingThread->start();
}

void MainWindow::onConversionFinished()
{
    if (processingThread) {
        if (processingThread->isRunning()) {
            processingThread->quit();
            processingThread->wait();
        }
        delete processingThread;
        processingThread = nullptr;
    }
    if (processor) {
        delete processor;
        processor = nullptr;
    }
    
    // 恢复UI元素
    browseButton->setEnabled(true);
    convertButton->setEnabled(true);
    widthEdit->setEnabled(true);
    grayPropertyComboBox->setEnabled(true);
    channelOrderComboBox->setEnabled(true);
    progressBar->setVisible(false);
    
    appendLog(QString::fromUtf8("转换完成! 请查看选择的文件夹下是否生成xyz_result文件夹。"));
    QMessageBox::information(this, QString::fromUtf8("完成"), QString::fromUtf8("PLY到TIFF转换已完成!"));
}

void MainWindow::onConversionProgress(int value)
{
    progressBar->setValue(value);
}

void MainWindow::onAdvancedBrowseButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, QString::fromUtf8("选择包含PLY文件的文件夹"), QDir::homePath());
    if (folderPath.isEmpty()) {
        return;
    }

    advancedInputFolderPath = folderPath;
    advancedInputFolderEdit->setText(folderPath);
    advancedConvertButton->setEnabled(true);
    appendLog(QString::fromUtf8("已选择XYZ/灰度导出文件夹: ") + folderPath);
}

void MainWindow::onAdvancedStartConversionClicked()
{
    if (advancedInputFolderPath.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择输入文件夹!"));
        return;
    }

    bool widthOk = false;
    bool heightOk = false;
    int width = advancedWidthEdit->text().toInt(&widthOk);
    int height = advancedHeightEdit->text().toInt(&heightOk);

    if (!widthOk || width <= 0 || !heightOk || height <= 0) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入合法的宽度与高度!"));
        return;
    }

    advancedBrowseButton->setEnabled(false);
    advancedConvertButton->setEnabled(false);
    advancedWidthEdit->setEnabled(false);
    advancedHeightEdit->setEnabled(false);
    advancedProgressBar->setVisible(true);
    advancedProgressBar->setValue(0);

    advancedProcessingThread = new QThread(this);
    advancedProcessor = new AdvancedPlyProcessor();
    advancedProcessor->moveToThread(advancedProcessingThread);

    connect(advancedProcessingThread, &QThread::started, [=]() {
        advancedProcessor->process(advancedInputFolderPath, width, height);
    });
    connect(advancedProcessor, &AdvancedPlyProcessor::progressUpdated, this, &MainWindow::onAdvancedConversionProgress);
    connect(advancedProcessor, &AdvancedPlyProcessor::finished, this, &MainWindow::onAdvancedConversionFinished);
    connect(advancedProcessor, &AdvancedPlyProcessor::logMessage, this, &MainWindow::appendLog);

    appendLog(QString::fromUtf8("开始XYZ/灰度导出流程..."));
    advancedProcessingThread->start();
}

void MainWindow::onAdvancedConversionFinished()
{
    if (advancedProcessingThread) {
        if (advancedProcessingThread->isRunning()) {
            advancedProcessingThread->quit();
            advancedProcessingThread->wait();
        }
        delete advancedProcessingThread;
        advancedProcessingThread = nullptr;
    }
    if (advancedProcessor) {
        delete advancedProcessor;
        advancedProcessor = nullptr;
    }

    advancedBrowseButton->setEnabled(true);
    advancedConvertButton->setEnabled(!advancedInputFolderPath.isEmpty());
    advancedWidthEdit->setEnabled(true);
    advancedHeightEdit->setEnabled(true);
    advancedProgressBar->setVisible(false);

    appendLog(QString::fromUtf8("XYZ/灰度导出完成，结果位于所选目录的 ply_tiff_output 文件夹。"));
    QMessageBox::information(this, QString::fromUtf8("完成"), QString::fromUtf8("XYZ/灰度导出已完成!"));
}

void MainWindow::onAdvancedConversionProgress(int value)
{
    advancedProgressBar->setValue(value);
}

void MainWindow::appendLog(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString coloredMessage;
    
    // 根据消息内容设置不同的颜色
    if (message.contains(QString::fromUtf8("欢迎")) || message.contains(QString::fromUtf8("完成"))) {
        coloredMessage = QString("<span style='color: #2ecc71;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(QString::fromUtf8("警告")) || message.contains(QString::fromUtf8("错误")) || message.contains(QString::fromUtf8("失败"))) {
        coloredMessage = QString("<span style='color: #e74c3c;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(QString::fromUtf8("开始")) || message.contains(QString::fromUtf8("处理"))) {
        coloredMessage = QString("<span style='color: #3498db;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else if (message.contains(QString::fromUtf8("已加载")) || message.contains(QString::fromUtf8("已选择"))) {
        coloredMessage = QString("<span style='color: #9b59b6;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    } else {
        coloredMessage = QString("<span style='color: #ecf0f1;'><b>[%1]</b> %2</span>").arg(timestamp, message);
    }
    
    logTextEdit->append(coloredMessage);
    
    // 将日志写入文件
    writeLogToFile(message);
}

void MainWindow::loadPropertyNames(const QString& folderPath)
{
    if (folderPath.isEmpty()) {
        return;
    }
    
    // 清空下拉框
    grayPropertyComboBox->clear();
    
    // 递归查找PLY文件
    QStringList filters;
    filters << "*.ply";
    QDirIterator it(folderPath, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    QStringList filePaths;
    while (it.hasNext()) {
        filePaths << it.next();
    }
    
    if (filePaths.isEmpty()) {
        appendLog(QString::fromUtf8("未找到PLY文件，无法加载属性名称"));
        return;
    }
    
    // 读取第一个PLY文件的属性名称
    PlyProcessor tempProcessor;
    QString firstPlyFile = filePaths.first();
    std::vector<std::string> properties = tempProcessor.getPropertyNames(firstPlyFile.toStdString());
    
    if (properties.empty()) {
        appendLog(QString::fromUtf8("无法读取PLY文件属性，请检查文件是否有效"));
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
    
    appendLog(QString::fromUtf8("已加载 ") + QString::number(properties.size()) + QString::fromUtf8(" 个属性: ") + QFileInfo(firstPlyFile).fileName());
}

// 初始化日志系统
void MainWindow::initLogSystem()
{
    // 获取程序运行目录
    QString appDirPath = QCoreApplication::applicationDirPath();
    logFolderPath = appDirPath + "/log";
    
    // 创建log文件夹
    QDir dir;
    if (!dir.exists(logFolderPath)) {
        if (!dir.mkpath(logFolderPath)) {
            // 如果创建失败，禁用日志文件功能
            return;
        }
    }
    
    // 获取当前日期的日志文件路径
    QString logFilePath = getLogFilePath();
    
    // 打开日志文件
    logFile = new QFile(logFilePath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        logStream = new QTextStream(logFile);
        logStream->setCodec("UTF-8");  // 设置编码为UTF-8
        
        // 写入分隔符，标记新的运行会话
        *logStream << "\n";
        *logStream << "========================================\n";
        *logStream << QString::fromUtf8("程序启动时间: ") << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        *logStream << "========================================\n";
        logStream->flush();
    }
}

// 获取当前日期的日志文件路径
QString MainWindow::getLogFilePath()
{
    QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
    return logFolderPath + "/" + currentDate + ".log";
}

// 写入日志到文件
void MainWindow::writeLogToFile(const QString& message)
{
    if (!logStream || !logFile || !logFile->isOpen()) {
        return;
    }
    
    // 检查日期是否变化，如果变化则切换到新的日志文件
    QString currentLogFile = getLogFilePath();
    if (currentLogFile != logFile->fileName()) {
        // 关闭当前文件
        logStream->flush();
        delete logStream;
        logFile->close();
        delete logFile;
        
        // 打开新日志文件
        logFile = new QFile(currentLogFile);
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            logStream = new QTextStream(logFile);
            logStream->setCodec("UTF-8");
            
            *logStream << "\n";
            *logStream << "========================================\n";
            *logStream << QString::fromUtf8("新日期会话: ") << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
            *logStream << "========================================\n";
        } else {
            logStream = nullptr;
            return;
        }
    }
    
    // 写入日志
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    *logStream << "[" << timestamp << "] " << message << "\n";
    logStream->flush();  // 立即刷新到文件
}
