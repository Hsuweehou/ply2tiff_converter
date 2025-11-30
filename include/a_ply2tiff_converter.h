/**
 * @file a_ply2tiff_converter.h
 * @brief A系列面阵相机 PLY 到 TIFF 转换器
 * 
 * 该模块用于处理A系列面阵相机的点云数据转换：
 * - 自动读取 PLY 文件中的 x/y/z 坐标属性
 * - 自动读取 red/green/blue 颜色属性
 * - 输出 CV_32FC3 格式的 XYZ 坐标 TIFF 图像
 * - 输出 CV_8UC3 格式的灰度 TIFF 图像（三通道，RGB值相同）
 * - 支持自定义图像宽度和高度
 */

#pragma once
#ifndef NOMINMAX                      
#  define NOMINMAX
#endif
#include <cstddef>
#include <functional>
#include <string>
#include <memory>
#include <vector>

namespace advanced_ply {

struct ConvertOptions {
    std::string inputFolder;
    int width = 0;
    int height = 0;
};

using LogCallback = std::function<void(const std::string&)>;
using ProgressCallback = std::function<void(std::size_t current, std::size_t total)>;

class AdvancedPlyConverter {
public:
    explicit AdvancedPlyConverter(const ConvertOptions& options);
    
    void setLogCallback(LogCallback callback);
    void setProgressCallback(ProgressCallback callback);
    
    void convert();
    
    // 便捷静态方法，保持向后兼容
    static void convertFolder(const ConvertOptions& options,
                              const LogCallback& logCallback = LogCallback{},
                              const ProgressCallback& progressCallback = ProgressCallback{});

private:
    struct PlyBuffers;
    
    ConvertOptions options_;
    LogCallback logCallback_;
    ProgressCallback progressCallback_;
    
    void ensureDirectory(const std::string& path) const;
    std::vector<std::string> collectPlyFiles(const std::string& folder) const;
    std::unique_ptr<PlyBuffers> loadPly(const std::string& plyPath) const;
    void writeTiffImages(const PlyBuffers& buffers,
                         const std::string& xyzPath,
                         const std::string& grayPath) const;
};

// 向后兼容：保留函数接口
inline void convertFolder(const ConvertOptions& options,
                          const LogCallback& logCallback = LogCallback{},
                          const ProgressCallback& progressCallback = ProgressCallback{}) {
    AdvancedPlyConverter::convertFolder(options, logCallback, progressCallback);
}

}  // namespace advanced_ply

