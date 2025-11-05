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
#include <chrono>   //计算时间
#include "../include/happly.h"

//namespace ply2tiff_converter {

#define DOWN_SAMPLE 1

using namespace std::chrono;
// 假设点云为有序结构，每个像素对应一个点
void save_ply2tiff(std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords,
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
    std::vector<int> compression_params = { cv::IMWRITE_TIFF_COMPRESSION, 1 }; // 可选压缩参数
    cv::imwrite(filename, tiff_image, compression_params);
}

cv::Mat save_gray2tiff(std::vector<uint8_t>& data, int width, int height, const std::string& filename) {
    std::string save_filename = filename;

    cv::Mat tiff_image(height, width, CV_8UC1, data.data());

    // 保存TIFF文件
    std::vector<int> compression_params = { cv::IMWRITE_TIFF_COMPRESSION, 1 }; // 可选压缩参数
    cv::imwrite(save_filename, tiff_image, compression_params);
    return tiff_image;
}

// 使用字符分割
void Stringsplit_str(const std::string& str, const char split, std::vector<std::string>& res)
{
    if (str == "")		return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + split;
    size_t pos = strs.find(split);

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(split);
    }
}
// 使用字符串分割
void Stringsplit_string(const std::string& str, const std::string& splits, std::vector<std::string>& res)
{
    if (str == "")		return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}


void getFiles_with_tail(std::string path, std::vector<std::string>& files, std::vector<std::string>& file_name, const char* sType)
{
    //文件句柄
    //long hFile = 0;
    intptr_t hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,迭代之
            //如果不是,加入列表
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
                    // std::cout << "fileinfo.name: " << fileinfo.name << "\n";
                }
            }

        } while (_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }
}

int save_tiff(const std::string& filename, const std::string& ply_tiff, const std::string& gray_tiff, int img_width) {

    std::cout << "saving tiff\n";

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
        //std::vector<uint8_t> read_gray = test.getVertexColors();

        std::vector<uint8_t> read_gray = test.getElement("vertex").getProperty<uint8_t>("gray");
        //std::wcout << read_gray.size() << "\n";
        for (int j = 0; j < read_ply.size(); j++) {
            // AIeveR_Point3F p(0, 0, 0);
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
            if (j % (3200 / img_width) != 0 && img_width != 3200) {
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
        std::cout << "plys[i].size(): " << plys[i].size() << "\n";
    }
    auto starttime = system_clock::now();
    //分别读取
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
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - starttime).count();
    std::cout << "times: " << diff << " ms\n";

    return 0;
}

bool create_directory_recursive(const std::string& path) {
    try {
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) {
                std::cout << "Directory already exists: " << path << "\n";
                return true;
            }
            else {
                std::cerr << "Path exists but is not a directory: " << path << "\n";
                return false;
            }
        }

        if (std::filesystem::create_directories(path)) {
            std::cout << "Successfully created directory: " << path << "\n";
            return true;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "Unknown error occurred\n";
    }
    return false;
}

//保存点云到tiff文件
int main_test(int argc, char** argv) {

    std::string username;
    bool show_version = false;

    if (argc <= 2) {
        std::cerr << "Usage: " << argv[0] << " <input_path>" << " <x_width(int)>\n";
        return 1;
    }

    std::string input_path = argv[1];
    int x_width = atoi(argv[2]);
    std::cout << "x_width: " << x_width << "\n";
    std::string output_path = input_path + "\\xyz_result";
    if (!create_directory_recursive(output_path)) {
        return 1;
    }
    std::cout << "Input: " << input_path << "\n";
    std::cout << "Output: " << output_path << "\n";

    std::cout << "reading ply file...\n";
    std::string folderPath = input_path;
    char* folderTail = ".ply";
    std::vector<std::string> ply_file_paths;
    std::vector<std::string> ply_files;
    getFiles_with_tail(folderPath, ply_file_paths, ply_files, folderTail);
    for (int i = 0; i < ply_file_paths.size(); i++) {
        std::cout << "filepath: " << ply_file_paths[i] << "\n";
        std::cout << "ply_files: " << ply_files[i] << "\n";
        std::vector<std::string> strList;
        Stringsplit_str(ply_files[i], '.', strList);
        ply_files[i] = strList[0];
    }
    for (int j = 0; j < ply_file_paths.size(); j++) {
        save_tiff(ply_file_paths[j], output_path + "\\" + ply_files[j] + "_xyz.tiff", output_path + "\\" + ply_files[j] + "_gray.tiff", x_width);
    }
}