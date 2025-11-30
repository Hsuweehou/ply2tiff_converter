/**
 * @file advanced_ply_processor.h
 * @brief A系列面阵相机 Qt 处理器
 * 
 * 该处理器用于在Qt多线程环境中处理A系列面阵相机的点云转换任务。
 * 封装了 AdvancedPlyConverter 类，提供Qt信号槽机制用于进度更新和日志输出。
 */

#pragma once

#include <QObject>
#include <QString>

class AdvancedPlyProcessor : public QObject
{
    Q_OBJECT

public:
    explicit AdvancedPlyProcessor(QObject* parent = nullptr);

public slots:
    void process(const QString& folderPath, int width, int height);

signals:
    void progressUpdated(int value);
    void finished();
    void logMessage(const QString& message);
};

