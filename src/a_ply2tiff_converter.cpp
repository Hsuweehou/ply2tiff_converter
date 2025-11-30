/**
 * @file a_ply2tiff_converter.cpp
 * @brief A系列面阵相机 PLY 到 TIFF 转换器实现
 * 
 * 实现 A系列面阵相机的点云转换功能，支持：
 * - 批量处理文件夹中的所有 PLY 文件
 * - 自定义输出图像尺寸（宽度 x 高度）
 * - 进度回调和日志回调
 */

#include "a_ply2tiff_converter.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <filesystem>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <io.h>
#include <windows.h>
#include <tchar.h>
#include <unordered_map>
#include <functional>
#include <chrono>
#include "happly.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <memory>

namespace advanced_ply {

// PlyBuffers 结构体定义
struct AdvancedPlyConverter::PlyBuffers {
    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> zs;
    std::vector<uint8_t> rs;
    std::vector<uint8_t> gs;
    std::vector<uint8_t> bs;
};

constexpr int kTiffCompression = 1;  // 无压缩，便于分析

// 构造函数
AdvancedPlyConverter::AdvancedPlyConverter(const ConvertOptions& options)
    : options_(options)
{
}

// 设置日志回调
void AdvancedPlyConverter::setLogCallback(LogCallback callback) {
    logCallback_ = callback;
}

// 设置进度回调
void AdvancedPlyConverter::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = callback;
}

// 确保目录存在
void AdvancedPlyConverter::ensureDirectory(const std::string& path) const {
    std::filesystem::path dirPath(path);
    if (std::filesystem::exists(dirPath)) {
        if (!std::filesystem::is_directory(dirPath)) {
            throw std::runtime_error("路径存在但不是文件夹: " + path);
        }
        return;
    }

    if (!std::filesystem::create_directories(dirPath)) {
        throw std::runtime_error("无法创建输出文件夹: " + path);
    }
}

// 收集PLY文件
std::vector<std::string> AdvancedPlyConverter::collectPlyFiles(const std::string& folder) const {
    std::filesystem::path folderPath(folder);
    if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
        throw std::runtime_error("输入路径不是有效文件夹: " + folder);
    }

    std::vector<std::string> files;
    for (auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        if (entry.path().extension() == ".ply") {
            files.push_back(entry.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

// 加载PLY文件
std::unique_ptr<AdvancedPlyConverter::PlyBuffers> AdvancedPlyConverter::loadPly(const std::string& plyPath) const {
    happly::PLYData plyData(plyPath);
    auto& vertex = plyData.getElement("vertex");

    auto buffers = std::make_unique<PlyBuffers>();
    buffers->xs = vertex.getProperty<double>("x");
    buffers->ys = vertex.getProperty<double>("y");
    buffers->zs = vertex.getProperty<double>("z");
    buffers->rs = vertex.getProperty<uint8_t>("red");
    buffers->gs = vertex.getProperty<uint8_t>("green");
    buffers->bs = vertex.getProperty<uint8_t>("blue");

    return buffers;
}

// 写入TIFF图像
void AdvancedPlyConverter::writeTiffImages(const PlyBuffers& buffers,
                                           const std::string& xyzPath,
                                           const std::string& grayPath) const {
    const size_t pixelCount = static_cast<size_t>(options_.width) * static_cast<size_t>(options_.height);
    const size_t pointCount = std::min({buffers.xs.size(), buffers.ys.size(), buffers.zs.size(),
                                       buffers.rs.size(), buffers.gs.size(), buffers.bs.size()});

    if (pointCount < pixelCount) {
        std::string warning = "[警告] 点数(" + std::to_string(pointCount) + 
                            ")少于图像像素(" + std::to_string(pixelCount) + ")，剩余像素将填0";
        if (logCallback_) {
            logCallback_(warning);
        } else {
            std::cerr << warning << "\n";
        }
    } else if (pointCount > pixelCount) {
        std::string warning = "[警告] 点数(" + std::to_string(pointCount) + 
                            ")多于图像像素(" + std::to_string(pixelCount) + ")，将自动裁剪";
        if (logCallback_) {
            logCallback_(warning);
        } else {
            std::cerr << warning << "\n";
        }
    }

    cv::Mat xyz(options_.height, options_.width, CV_32FC3, cv::Scalar(0.0f, 0.0f, 0.0f));
    cv::Mat gray(options_.height, options_.width, CV_8UC3, cv::Scalar(0, 0, 0));

    const size_t copyCount = std::min(pointCount, pixelCount);
    for (size_t idx = 0; idx < copyCount; ++idx) {
        const int row = static_cast<int>(idx / options_.width);
        const int col = static_cast<int>(idx % options_.width);

        cv::Vec3f& xyzPixel = xyz.at<cv::Vec3f>(row, col);
        xyzPixel[0] = static_cast<float>(buffers.xs[idx]);
        xyzPixel[1] = static_cast<float>(buffers.ys[idx]);
        xyzPixel[2] = static_cast<float>(buffers.zs[idx]);

        constexpr float kR = 0.299f;
        constexpr float kG = 0.587f;
        constexpr float kB = 0.114f;
        const float grayVal = kR * buffers.rs[idx] +
                             kG * buffers.gs[idx] +
                             kB * buffers.bs[idx];
        const uint8_t grayU8 = static_cast<uint8_t>(std::clamp(grayVal, 0.0f, 255.0f));
        cv::Vec3b& grayPixel = gray.at<cv::Vec3b>(row, col);
        grayPixel[0] = grayU8;
        grayPixel[1] = grayU8;
        grayPixel[2] = grayU8;
    }

    std::vector<int> params = {cv::IMWRITE_TIFF_COMPRESSION, kTiffCompression};
    if (!cv::imwrite(xyzPath, xyz, params)) {
        throw std::runtime_error("写入XYZ TIFF失败: " + xyzPath);
    }
    if (!cv::imwrite(grayPath, gray, params)) {
        throw std::runtime_error("写入灰度TIFF失败: " + grayPath);
    }
}

// 执行转换
void AdvancedPlyConverter::convert() {
    if (options_.inputFolder.empty()) {
        throw std::invalid_argument("输入路径不能为空");
    }
    if (options_.width <= 0 || options_.height <= 0) {
        throw std::invalid_argument("图像宽高必须为正整数");
    }

    const auto plyFiles = collectPlyFiles(options_.inputFolder);
    if (plyFiles.empty()) {
        throw std::runtime_error("未在目录中找到PLY文件: " + options_.inputFolder);
    }

    std::filesystem::path outputDir = std::filesystem::path(options_.inputFolder) / "ply_tiff_output";
    ensureDirectory(outputDir.string());

    if (logCallback_) {
        logCallback_("输出目录: " + outputDir.string());
        logCallback_("共有 " + std::to_string(plyFiles.size()) + " 个PLY文件待处理");
    }

    std::size_t processed = 0;
    for (const auto& plyFile : plyFiles) {
        std::filesystem::path plyPath(plyFile);
        if (logCallback_) {
            logCallback_("开始处理: " + plyPath.filename().string());
        }

        auto buffers = loadPly(plyFile);
        std::filesystem::path xyzPath = outputDir / (plyPath.stem().string() + "_xyz.tiff");
        std::filesystem::path grayPath = outputDir / (plyPath.stem().string() + "_gray.tiff");
        writeTiffImages(*buffers, xyzPath.string(), grayPath.string());

        if (logCallback_) {
            logCallback_("完成: " + plyPath.filename().string());
        }

        ++processed;
        if (progressCallback_) {
            progressCallback_(processed, plyFiles.size());
        }
    }

    if (logCallback_) {
        logCallback_("全部完成，输出目录: " + outputDir.string());
    }
}

// 静态方法，保持向后兼容
void AdvancedPlyConverter::convertFolder(const ConvertOptions& options,
                                         const LogCallback& logCallback,
                                         const ProgressCallback& progressCallback) {
    AdvancedPlyConverter converter(options);
    converter.setLogCallback(logCallback);
    converter.setProgressCallback(progressCallback);
    converter.convert();
}

}  // namespace advanced_ply

#ifdef ADVANCED_PLY_CONVERTER_STANDALONE
int main(int argc, char** argv) {
    try {
        if (argc < 4) {
            std::cerr << "用法: " << argv[0]
                      << " <ply文件夹> <图像宽度> <图像高度>\n";
            return 1;
        }

        advanced_ply::ConvertOptions options;
        options.inputFolder = argv[1];
        options.width = std::stoi(argv[2]);
        options.height = std::stoi(argv[3]);

        advanced_ply::convertFolder(
            options,
            [](const std::string& message) {
                std::cout << message << "\n";
            },
            [](std::size_t current, std::size_t total) {
                if (total == 0) {
                    return;
                }
                const int percent = static_cast<int>((100.0 * current) / static_cast<double>(total));
                std::cout << "进度: " << percent << "% (" << current << "/" << total << ")\n";
            });
    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
#endif

