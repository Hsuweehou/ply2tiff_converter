#include "ply_processor.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <iostream>
#include <filesystem>
#include <io.h>
#define NOMINMAX
#include <windows.h>

PlyProcessor::PlyProcessor(QObject *parent)
    : QObject(parent)
{
}

void PlyProcessor::process(const std::string& inputPath, int imgWidth)
{
    emit logMessage("开始处理路径: " + QString::fromStdString(inputPath));
    emit logMessage("图像宽度设置为: " + QString::number(imgWidth));
    
    std::string output_path = inputPath + "\\xyz_result";
    if (!create_directory_recursive(output_path)) {
        emit logMessage("创建输出目录失败");
        emit finished();
        return;
    }
    
    emit logMessage("输出目录: " + QString::fromStdString(output_path));
    emit logMessage("正在扫描PLY文件...");
    
    std::string folderPath = inputPath;
    char* folderTail = ".ply";
    std::vector<std::string> ply_file_paths;
    std::vector<std::string> ply_files;
    getFiles_with_tail(folderPath, ply_file_paths, ply_files, folderTail);
    
    emit logMessage("找到 " + QString::number(ply_file_paths.size()) + " 个PLY文件");
    
    for (int i = 0; i < ply_file_paths.size(); i++) {
        emit logMessage("处理文件: " + QString::fromStdString(ply_file_paths[i]));
        std::vector<std::string> strList;
        Stringsplit_str(ply_files[i], '.', strList);
        if (!strList.empty()) {
            ply_files[i] = strList[0];
        }
    }
    
    // 更新进度
    emit progressUpdated(10);
    
    for (int j = 0; j < ply_file_paths.size(); j++) {
        emit logMessage("转换 " + QString::fromStdString(ply_files[j]) + "...");
        save_tiff(ply_file_paths[j], output_path + "\\" + ply_files[j] + "_xyz.tiff", output_path + "\\" + ply_files[j] + "_gray.tiff", imgWidth);
        
        // 更新进度
        int progress = 10 + (j + 1) * 90 / std::max(1, (int)ply_file_paths.size());
        emit progressUpdated(progress);
    }
    
    emit logMessage("所有文件处理完成");
    emit finished();
}

void PlyProcessor::save_ply2tiff(std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords,
                                 int width, int height, const std::string& filename) {
    cv::Mat tiff_image(height, width, CV_32FC3);

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int idx = row * width + col;
            cv::Vec3f& pixel = tiff_image.at<cv::Vec3f>(row, col);
            pixel[0] = x_coords[idx];  // X -> 通道2 - b
            pixel[1] = y_coords[idx];  // Y -> 通道1 - g
            pixel[2] = z_coords[idx];  // Z -> 通道0 - r
        }
    }

    // 保存TIFF文件
    std::vector<int> compression_params = { cv::IMWRITE_TIFF_COMPRESSION, 1 }; // 无压缩
    cv::imwrite(filename, tiff_image, compression_params);
}

cv::Mat PlyProcessor::save_gray2tiff(std::vector<uint8_t>& data, int width, int height, const std::string& filename) {
    std::string save_filename = filename;

    cv::Mat tiff_image(height, width, CV_8UC1, data.data());

    // 保存TIFF文件
    std::vector<int> compression_params = { cv::IMWRITE_TIFF_COMPRESSION, 1 }; // 无压缩
    cv::imwrite(save_filename, tiff_image, compression_params);
    return tiff_image;
}

// 使用字符分割
void PlyProcessor::Stringsplit_str(const std::string& str, const char split, std::vector<std::string>& res)
{
    if (str == "") return;
    // 在字符串末尾也加上分割符，方便截取最后一段
    std::string strs = str + split;
    size_t pos = strs.find(split);

    // 若找不到分割符，则返回npos
    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        // 去掉已分割的字符串,对剩余的字符串进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(split);
    }
}

void PlyProcessor::getFiles_with_tail(std::string path, std::vector<std::string>& files, std::vector<std::string>& file_name, const char* sType)
{
    intptr_t hFile = 0;
    // 文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            // 如果是目录,则递归查找
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    getFiles_with_tail(p.assign(path).append("\\").append(fileinfo.name), files, file_name, sType);
                }
            }
            else
            {
                char* pName = fileinfo.name;
                char* pFind = strstr(pName, sType);
                if (pFind != NULL)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    file_name.push_back(fileinfo.name);
                }
            }

        } while (_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }
}

int PlyProcessor::save_tiff(const std::string& filename, const std::string& ply_tiff, const std::string& gray_tiff, int img_width) {
    emit logMessage("保存TIFF文件...");

    std::vector<std::vector<cv::Point3f>> plys;
    std::vector<float> plys_x;
    std::vector<float> plys_y;
    std::vector<float> plys_z;
    std::vector<std::vector<uint8_t>> grays;
    std::vector<uint8_t> grays_elem;
    std::vector<std::string> ply_file_paths;

    ply_file_paths.push_back(filename);

    plys.resize(1);
    plys_x.resize(1);
    plys_y.resize(1);
    plys_z.resize(1);
    grays.resize(1);
    grays_elem.resize(1);
    for (int i = 0; i < ply_file_paths.size(); i++) {
        happly::PLYData test(ply_file_paths[i]);
        std::vector<std::array<double, 3>> read_ply = test.getVertexPositions();
        std::vector<uint8_t> read_gray = test.getElement("vertex").getProperty<uint8_t>("gray");
        for (int j = 0; j < read_ply.size(); j++) {
            if (img_width == 3200) {
                cv::Point3f p(0, 0, 0);
                plys_x.push_back(read_ply[j][0]);
                plys_y.push_back(read_ply[j][1]);
                plys_z.push_back(read_ply[j][2]);
                p.x = read_ply[j][0];
                p.y = read_ply[j][1];
                p.z = read_ply[j][2];
                plys[i].push_back(p);
                grays_elem.push_back(read_gray[j]);
                grays[i].push_back(read_gray[j]);
            }
            // 注意：原代码中的逻辑可能存在问题，这里进行了调整
            else if (j % (3200 / img_width) != 0) {
                cv::Point3f p(0, 0, 0);
                plys_x.push_back(read_ply[j][0]);
                plys_y.push_back(read_ply[j][1]);
                plys_z.push_back(read_ply[j][2]);
                p.x = read_ply[j][0];
                p.y = read_ply[j][1];
                p.z = read_ply[j][2];
                plys[i].push_back(p);
                grays_elem.push_back(read_gray[j]);
                grays[i].push_back(read_gray[j]);
            }
            else
                continue;
        }
        emit logMessage("处理了 " + QString::number(plys[i].size()) + " 个点");
    }
    
    // 分别获取
    for (int k = 0; k < ply_file_paths.size(); k++) {
        for (int i = 0; i < plys[k].size(); i++) {
                plys_x.push_back(plys[k][i].x);
                plys_y.push_back(plys[k][i].y);
                plys_z.push_back(plys[k][i].z);
                grays_elem.push_back(grays[k][i]);
        }
    }

    save_ply2tiff(plys_x, plys_y, plys_z, img_width, plys_z.size() / img_width / 2, ply_tiff);
    save_gray2tiff(grays_elem, img_width, plys_z.size() / img_width / 2, gray_tiff);

    return 0;
}

bool PlyProcessor::create_directory_recursive(const std::string& path) {
    try {
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) {
                return true;
            }
            else {
                return false;
            }
        }

        if (std::filesystem::create_directories(path)) {
            return true;
        }
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }
    catch (...) {
        return false;
    }
    return false;
}