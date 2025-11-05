#include "mainwindow.h"
#include "ui_strings.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QHeaderView>
#include <QTreeWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , navigationTree(nullptr)
    , contentStack(nullptr)
    , contentTitleLabel(nullptr)
    , plyTiffConverterPage(nullptr)
{
    setWindowTitle(UIStrings::cameraService());
    resize(1200, 700);
    setMinimumSize(1000, 600);
    
    // 设置窗口图标
    setWindowIcon(QIcon(":/icons/app.ico"));
    
    setupUI();
    setupNavigation();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 创建中央部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局：横向布局
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 左侧导航栏
    QWidget* navigationWidget = new QWidget(this);
    navigationWidget->setFixedWidth(280);
    navigationWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #1e3c72, stop:1 #2a5298);"
        "}"
    );
    
    QVBoxLayout* navLayout = new QVBoxLayout(navigationWidget);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(0);
    
    // 标题区域
    QWidget* titleWidget = new QWidget(navigationWidget);
    titleWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(0, 0, 0, 0.2);"
        //"    border-bottom: 1px solid #3498db;"
        "}"
    );
    QVBoxLayout* titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(20, 25, 20, 25);
    titleLayout->setSpacing(5);
    
    QLabel* appNameLabel = new QLabel(UIStrings::cameraService(), titleWidget);
    appNameLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffffff;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "}"
    );
    appNameLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* versionLabel = new QLabel("v1.0.3", titleWidget);
    versionLabel->setStyleSheet(
        "QLabel {"
        "    color: #bdc3c7;"
        "    font-size: 11px;"
        "    background: transparent;"
        "}"
    );
    versionLabel->setAlignment(Qt::AlignCenter);
    
    titleLayout->addWidget(appNameLabel);
    titleLayout->addWidget(versionLabel);
    
    // 导航树
    navigationTree = new QTreeWidget(navigationWidget);
    navigationTree->setHeaderHidden(true);
    navigationTree->setIndentation(25);
    navigationTree->setAnimated(true);
    navigationTree->setStyleSheet(
        "QTreeWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "    color: #ecf0f1;"
        "    font-size: 14px;"
        "    padding: 10px 0px;"
        "}"
        "QTreeWidget::item {"
        "    padding: 12px 20px;"
        "    border: none;"
        "    border-left: 3px solid transparent;"
        "    margin: 2px 0px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgba(255, 255, 255, 0.1);"
        "    border-left: 3px solid #3498db;"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: rgba(52, 152, 219, 0.3);"
        "    border-left: 3px solid #3498db;"
        "    color: #ffffff;"
        "    font-weight: bold;"
        "}"
        "QTreeWidget::branch {"
        "    background: transparent;"
        "}"
        "QTreeWidget::branch:has-children:!has-siblings:closed,"
        "QTreeWidget::branch:closed:has-children:has-siblings {"
        "    border: none;"
        "    image: url(none);"
        "}"
        "QTreeWidget::branch:open:has-children:!has-siblings,"
        "QTreeWidget::branch:open:has-children:has-siblings {"
        "    border: none;"
        "    image: url(none);"
        "}"
    );
    
    navLayout->addWidget(titleWidget);
    navLayout->addWidget(navigationTree);
    navLayout->addStretch();
    
    // 右侧内容区域
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f6fa;"
        "}"
    );
    
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    // 内容页面标题栏
    QWidget* headerWidget = new QWidget(contentWidget);
    headerWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #ffffff, stop:1 #f8f9fa);"
        "    border-bottom: 1px solid #dee2e6;"
        "}"
    );
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(30, 20, 30, 20);
    
    contentTitleLabel = new QLabel(headerWidget);
    contentTitleLabel->setStyleSheet(
        "QLabel {"
        "    color: #2c3e50;"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "}"
    );
    
    headerLayout->addWidget(contentTitleLabel);
    headerLayout->addStretch();
    
    // 页面堆栈
    contentStack = new QStackedWidget(contentWidget);
    
    contentLayout->addWidget(headerWidget);
    contentLayout->addWidget(contentStack);
    
    // 添加到主布局
    mainLayout->addWidget(navigationWidget);
    mainLayout->addWidget(contentWidget, 1);
    
    // 连接信号和槽
    connect(navigationTree, &QTreeWidget::itemClicked, this, &MainWindow::onNavigationItemClicked);
}

void MainWindow::setupNavigation()
{
    // 创建功能分类
    QTreeWidgetItem* functionsItem = new QTreeWidgetItem(navigationTree);
    functionsItem->setText(0, UIStrings::functions());
    functionsItem->setExpanded(true);
    functionsItem->setFlags(functionsItem->flags() & ~Qt::ItemIsSelectable);
    
    // 添加PLY-TIFF转换器
    QTreeWidgetItem* plyTiffItem = new QTreeWidgetItem(functionsItem);
    plyTiffItem->setText(0, UIStrings::plyTiffConverter());
    plyTiffItem->setData(0, Qt::UserRole, "ply_tiff_converter");
    
    // 创建其它分类
    QTreeWidgetItem* othersItem = new QTreeWidgetItem(navigationTree);
    othersItem->setText(0, UIStrings::others());
    othersItem->setExpanded(true);
    othersItem->setFlags(othersItem->flags() & ~Qt::ItemIsSelectable);
    
    // 创建页面
    plyTiffConverterPage = new PlyTiffConverterWidget(this);
    contentStack->addWidget(plyTiffConverterPage);
    
    // 默认选中第一个页面
    navigationTree->setCurrentItem(plyTiffItem);
    contentStack->setCurrentWidget(plyTiffConverterPage);
    contentTitleLabel->setText(UIStrings::plyTiffConverter().trimmed());
}

void MainWindow::onNavigationItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    
    if (!item) {
        return;
    }
    
    QString pageId = item->data(0, Qt::UserRole).toString();
    
    if (pageId == "ply_tiff_converter") {
        contentStack->setCurrentWidget(plyTiffConverterPage);
        contentTitleLabel->setText(UIStrings::plyTiffConverter().trimmed());
    }
}
