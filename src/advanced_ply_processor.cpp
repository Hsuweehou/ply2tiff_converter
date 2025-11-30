/**
 * @file advanced_ply_processor.cpp
 * @brief A系列面阵相机 Qt 处理器实现
 * 
 * 实现 A系列面阵相机的Qt多线程处理逻辑
 */

#include "advanced_ply_processor.h"

#include "a_ply2tiff_converter.h"

#include <exception>

AdvancedPlyProcessor::AdvancedPlyProcessor(QObject* parent)
    : QObject(parent)
{
}

void AdvancedPlyProcessor::process(const QString& folderPath, int width, int height)
{
    try {
        advanced_ply::ConvertOptions options;
        options.inputFolder = folderPath.toStdString();
        options.width = width;
        options.height = height;

        advanced_ply::convertFolder(
            options,
            [this](const std::string& message) {
                emit logMessage(QString::fromStdString(message));
            },
            [this](std::size_t current, std::size_t total) {
                if (total == 0) {
                    emit progressUpdated(0);
                    return;
                }
                const int percent = static_cast<int>((100.0 * current) / static_cast<double>(total));
                emit progressUpdated(percent);
            });
    } catch (const std::exception& ex) {
        emit logMessage(QString::fromUtf8("错误: ") + QString::fromUtf8(ex.what()));
    } catch (...) {
        emit logMessage(QString::fromUtf8("发生未知错误"));
    }

    emit finished();
}

