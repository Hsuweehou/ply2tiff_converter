#ifndef PLY_PROCESSOR_H
#define PLY_PROCESSOR_H

#include <QObject>
#include <QString>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "../include/happly.h"

class PlyProcessor : public QObject
{
    Q_OBJECT

public:
    explicit PlyProcessor(QObject *parent = nullptr);
    
public slots:
    void process(const std::string& inputPath, int imgWidth);

signals:
    void progressUpdated(int value);
    void finished();
    void logMessage(const QString& message);

private:
    void save_ply2tiff(std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords,
                       int width, int height, const std::string& filename);
                       
    cv::Mat save_gray2tiff(std::vector<uint8_t>& data, int width, int height, const std::string& filename);
    
    void Stringsplit_str(const std::string& str, const char split, std::vector<std::string>& res);
    
    void getFiles_with_tail(std::string path, std::vector<std::string>& files, std::vector<std::string>& file_name, const char* sType);
    
    int save_tiff(const std::string& filename, const std::string& ply_tiff, const std::string& gray_tiff, int img_width);
    
    bool create_directory_recursive(const std::string& path);
};

#endif // PLY_PROCESSOR_H