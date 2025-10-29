#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QGroupBox>
#include <QApplication>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , processingThread(nullptr)
    , processor(nullptr)
{
    setWindowTitle("PLY-TIFF 转换器");
    resize(600, 400);

    // 创建中央部件和主布局
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // 输入设置区域
    QGroupBox* settingsGroup = new QGroupBox("转换设置", this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    
    // 输入文件夹选择
    QHBoxLayout* folderLayout = new QHBoxLayout();
    inputFolderLabel = new QLabel("输入文件夹:", this);
    inputFolderEdit = new QLineEdit(this);
    inputFolderEdit->setReadOnly(true);
    browseButton = new QPushButton("浏览...", this);
    
    folderLayout->addWidget(inputFolderLabel);
    folderLayout->addWidget(inputFolderEdit);
    folderLayout->addWidget(browseButton);
    
    // 宽度设置
    QHBoxLayout* widthLayout = new QHBoxLayout();
    widthLabel = new QLabel("图像宽度:", this);
    widthEdit = new QLineEdit(this);
    widthEdit->setText("3200");
    widthEdit->setMaximumWidth(100);
    
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(widthEdit);
    widthLayout->addStretch();
    
    settingsLayout->addLayout(folderLayout);
    settingsLayout->addLayout(widthLayout);
    
    // 开始转换按钮
    convertButton = new QPushButton("开始转换", this);
    convertButton->setEnabled(false);
    
    // 进度条
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    
    // 日志输出
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    
    // 添加到主布局
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(convertButton);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(new QLabel("转换日志:", this));
    mainLayout->addWidget(logTextEdit);
    
    // 连接信号和槽
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::onBrowseButtonClicked);
    connect(convertButton, &QPushButton::clicked, this, &MainWindow::onStartConversionClicked);
    
    // 初始化日志
    appendLog("欢迎使用PLY到TIFF转换器!");
    appendLog("该转换器仅支持转换PLY文件为TIFF格式(点云tiff与灰度tiff)!");
    appendLog("输入PLY文件必须包含gray属性!");
    appendLog("请选择包含PLY文件的文件夹并设置图像宽度。");
}

MainWindow::~MainWindow()
{
    if (processingThread) {
        processingThread->quit();
        processingThread->wait();
        delete processingThread;
    }
    if (processor) {
        delete processor;
    }
}

void MainWindow::onBrowseButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "选择包含PLY文件的文件夹", QDir::homePath());
    if (!folderPath.isEmpty()) {
        inputFolderPath = folderPath;
        inputFolderEdit->setText(folderPath);
        convertButton->setEnabled(true);
        appendLog("已选择文件夹: " + folderPath);
    }
}

void MainWindow::onStartConversionClicked()
{
    if (inputFolderPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择输入文件夹!");
        return;
    }
    
    bool ok;
    int width = widthEdit->text().toInt(&ok);
    if (!ok || width <= 0 || width > 3200 || typeid(width) != typeid(int)) {
        QMessageBox::warning(this, "警告", "请输入有效的图像宽度!");
        return;
    }
    
    // 禁用UI元素
    browseButton->setEnabled(false);
    convertButton->setEnabled(false);
    widthEdit->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // 创建处理器对象和线程
    processingThread = new QThread(this);
    processor = new PlyProcessor();
    processor->moveToThread(processingThread);
    
    // 连接信号和槽
    connect(processingThread, &QThread::started, [=]() {
        processor->process(inputFolderPath.toStdString(), width);
    });
    
    connect(processor, &PlyProcessor::progressUpdated, this, &MainWindow::onConversionProgress);
    connect(processor, &PlyProcessor::finished, this, &MainWindow::onConversionFinished);
    connect(processor, &PlyProcessor::logMessage, this, &MainWindow::appendLog);
    
    // 开始处理
    appendLog("开始转换过程...");
    processingThread->start();
}

void MainWindow::onConversionFinished()
{
    if (processingThread && processingThread->isRunning()) {
        processingThread->quit();
        processingThread->wait();
    }
    
    // 恢复UI元素
    browseButton->setEnabled(true);
    convertButton->setEnabled(true);
    widthEdit->setEnabled(true);
    progressBar->setVisible(false);
    
    appendLog("转换完成! 请查看选择的文件夹下是否生成xyz_result文件夹。");
    QMessageBox::information(this, "完成", "PLY到TIFF转换已完成!");
}

void MainWindow::onConversionProgress(int value)
{
    progressBar->setValue(value);
}

void MainWindow::appendLog(const QString& message)
{
    logTextEdit->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] " + message);
}