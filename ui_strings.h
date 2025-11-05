#ifndef UI_STRINGS_H
#define UI_STRINGS_H

#include <QString>

// UI strings defined using UTF-8 byte sequences to avoid MSVC encoding issues
class UIStrings
{
public:
    // PlyTiffConverterWidget strings
    static inline QString conversionSettings() { 
        return QString::fromUtf8("\xe8\xbd\xac\xe6\x8d\xa2\xe8\xae\xbe\xe7\xbd\xae"); // 转换设置
    }
    static inline QString inputFolder() { 
        return QString::fromUtf8("<span style='color: #3498db; font-weight: bold;'>|</span> \xe8\xbe\x93\xe5\x85\xa5\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9:"); // 输入文件夹:
    }
    static inline QString placeholderText() { 
        return QString::fromUtf8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe5\x8c\x85\xe5\x90\xabPLY\xe6\x96\x87\xe4\xbb\xb6\xe7\x9a\x84\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9..."); // 请选择包含PLY文件的文件夹...
    }
    static inline QString browse() { 
        return QString::fromUtf8("\xe6\xb5\x8f\xe8\xa7\x88"); // 浏览
    }
    static inline QString imageWidth() { 
        return QString::fromUtf8("<span style='color: #e67e22; font-weight: bold;'>|</span> \xe5\x9b\xbe\xe5\x83\x8f\xe5\xae\xbd\xe5\xba\xa6:"); // 图像宽度:
    }
    static inline QString grayProperty() { 
        return QString::fromUtf8("<span style='color: #9b59b6; font-weight: bold;'>|</span> \xe7\x81\xb0\xe5\xba\xa6\xe5\xb1\x9e\xe6\x80\xa7:"); // 灰度属性:
    }
    static inline QString startConversion() { 
        return QString::fromUtf8("\xe5\xbc\x80\xe5\xa7\x8b\xe8\xbd\xac\xe6\x8d\xa2"); // 开始转换
    }
    static inline QString conversionLog() { 
        return QString::fromUtf8("<span style='color: #27ae60; font-weight: bold;'>|</span> \xe8\xbd\xac\xe6\x8d\xa2\xe6\x97\xa5\xe5\xbf\x97:"); // 转换日志:
    }
    
    // Welcome messages
    static inline QString welcome() { 
        return QString::fromUtf8("\xe6\xac\xa2\xe8\xbf\x8e\xe4\xbd\xbf\xe7\x94\xa8PLY\xe5\x88\xb0TIFF\xe8\xbd\xac\xe6\x8d\xa2\xe5\x99\xa8!"); // 欢迎使用PLY到TIFF转换器!
    }
    static inline QString info1() { 
        return QString::fromUtf8("\xe8\xaf\xa5\xe8\xbd\xac\xe6\x8d\xa2\xe5\x99\xa8\xe4\xbb\x85\xe6\x94\xaf\xe6\x8c\x81\xe8\xbd\xac\xe6\x8d\xa2PLY\xe6\x96\x87\xe4\xbb\xb6\xe4\xb8\xbaTIFF\xe6\xa0\xbc\xe5\xbc\x8f(\xe7\x82\xb9\xe4\xba\x91tiff\xe4\xb8\x8e\xe7\x81\xb0\xe5\xba\xa6tiff)!"); // 该转换器仅支持转换PLY文件为TIFF格式(点云tiff与灰度tiff)!
    }
    static inline QString info2() { 
        return QString::fromUtf8("\xe8\xbe\x93\xe5\x85\xa5PLY\xe6\x96\x87\xe4\xbb\xb6\xe5\xbf\x85\xe9\xa1\xbb\xe5\x8c\x85\xe5\x90\xab\xe6\x8c\x87\xe5\xae\x9a\xe7\x9a\x84\xe7\x81\xb0\xe5\xba\xa6\xe5\xb1\x9e\xe6\x80\xa7!"); // 输入PLY文件必须包含指定的灰度属性!
    }
    static inline QString info3() { 
        return QString::fromUtf8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe5\x8c\x85\xe5\x90\xabPLY\xe6\x96\x87\xe4\xbb\xb6\xe7\x9a\x84\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9\xe5\xb9\xb6\xe8\xae\xbe\xe7\xbd\xae\xe5\x9b\xbe\xe5\x83\x8f\xe5\xae\xbd\xe5\xba\xa6\xe5\x92\x8c\xe7\x81\xb0\xe5\xba\xa6\xe5\xb1\x9e\xe6\x80\xa7\xe5\x90\x8d\xe7\xa7\xb0\xe3\x80\x82"); // 请选择包含PLY文件的文件夹并设置图像宽度和灰度属性名称。
    }
    
    // Dialog messages
    static inline QString selectFolder() { 
        return QString::fromUtf8("\xe9\x80\x89\xe6\x8b\xa9\xe5\x8c\x85\xe5\x90\xabPLY\xe6\x96\x87\xe4\xbb\xb6\xe7\x9a\x84\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9"); // 选择包含PLY文件的文件夹
    }
    static inline QString warning() { 
        return QString::fromUtf8("\xe8\xad\xa6\xe5\x91\x8a"); // 警告
    }
    static inline QString pleaseSelectFolder() { 
        return QString::fromUtf8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe8\xbe\x93\xe5\x85\xa5\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9!"); // 请选择输入文件夹!
    }
    static inline QString validWidth() { 
        return QString::fromUtf8("\xe8\xaf\xb7\xe8\xbe\x93\xe5\x85\xa5\xe6\x9c\x89\xe6\x95\x88\xe7\x9a\x84\xe5\x9b\xbe\xe5\x83\x8f\xe5\xae\xbd\xe5\xba\xa6!"); // 请输入有效的图像宽度!
    }
    static inline QString selectProperty() { 
        return QString::fromUtf8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe7\x81\xb0\xe5\xba\xa6\xe5\xb1\x9e\xe6\x80\xa7\xe5\x90\x8d\xe7\xa7\xb0!"); // 请选择灰度属性名称!
    }
    static inline QString selectedFolder() { 
        return QString::fromUtf8("\xe5\xb7\xb2\xe9\x80\x89\xe6\x8b\xa9\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9: "); // 已选择文件夹: 
    }
    static inline QString startProcess() { 
        return QString::fromUtf8("\xe5\xbc\x80\xe5\xa7\x8b\xe8\xbd\xac\xe6\x8d\xa2\xe8\xbf\x87\xe7\xa8\x8b..."); // 开始转换过程...
    }
    static inline QString complete() { 
        return QString::fromUtf8("\xe8\xbd\xac\xe6\x8d\xa2\xe5\xae\x8c\xe6\x88\x90! \xe8\xaf\xb7\xe6\x9f\xa5\xe7\x9c\x8b\xe9\x80\x89\xe6\x8b\xa9\xe7\x9a\x84\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9\xe4\xb8\x8b\xe6\x98\xaf\xe5\x90\xa6\xe7\x94\x9f\xe6\x88\x90xyz_result\xe6\x96\x87\xe4\xbb\xb6\xe5\xa4\xb9\xe3\x80\x82"); // 转换完成! 请查看选择的文件夹下是否生成xyz_result文件夹。
    }
    static inline QString completed() { 
        return QString::fromUtf8("\xe5\xae\x8c\xe6\x88\x90"); // 完成
    }
    static inline QString conversionCompleted() { 
        return QString::fromUtf8("PLY\xe5\x88\xb0TIFF\xe8\xbd\xac\xe6\x8d\xa2\xe5\xb7\xb2\xe5\xae\x8c\xe6\x88\x90!"); // PLY到TIFF转换已完成!
    }
    static inline QString noFileFound() { 
        return QString::fromUtf8("\xe6\x9c\xaa\xe6\x89\xbe\xe5\x88\xb0PLY\xe6\x96\x87\xe4\xbb\xb6\xef\xbc\x8c\xe6\x97\xa0\xe6\xb3\x95\xe5\x8a\xa0\xe8\xbd\xbd\xe5\xb1\x9e\xe6\x80\xa7\xe5\x90\x8d\xe7\xa7\xb0"); // 未找到PLY文件，无法加载属性名称
    }
    static inline QString cannotReadFile() { 
        return QString::fromUtf8("\xe6\x97\xa0\xe6\xb3\x95\xe8\xaf\xbb\xe5\x8f\x96PLY\xe6\x96\x87\xe4\xbb\xb6\xe5\xb1\x9e\xe6\x80\xa7\xef\xbc\x8c\xe8\xaf\xb7\xe6\xa3\x80\xe6\x9f\xa5\xe6\x96\x87\xe4\xbb\xb6\xe6\x98\xaf\xe5\x90\xa6\xe6\x9c\x89\xe6\x95\x88"); // 无法读取PLY文件属性，请检查文件是否有效
    }
    static inline QString loadedProperties() { 
        return QString::fromUtf8("\xe5\xb7\xb2\xe5\x8a\xa0\xe8\xbd\xbd "); // 已加载 
    }
    static inline QString properties() { 
        return QString::fromUtf8(" \xe4\xb8\xaa\xe5\xb1\x9e\xe6\x80\xa7: "); // 个属性: 
    }
    static inline QString programStartTime() { 
        return QString::fromUtf8("\xe7\xa8\x8b\xe5\xba\x8f\xe5\x90\xaf\xe5\x8a\xa8\xe6\x97\xb6\xe9\x97\xb4: "); // 程序启动时间: 
    }
    static inline QString newDateSession() { 
        return QString::fromUtf8("\xe6\x96\xb0\xe6\x97\xa5\xe6\x9c\x9f\xe4\xbc\x9a\xe8\xaf\x9d: "); // 新日期会话: 
    }
    
    // Keywords for log coloring
    static inline QString kwWelcome() { 
        return QString::fromUtf8("\xe6\xac\xa2\xe8\xbf\x8e"); // 欢迎
    }
    static inline QString kwComplete() { 
        return QString::fromUtf8("\xe5\xae\x8c\xe6\x88\x90"); // 完成
    }
    static inline QString kwWarning() { 
        return QString::fromUtf8("\xe8\xad\xa6\xe5\x91\x8a"); // 警告
    }
    static inline QString kwError() { 
        return QString::fromUtf8("\xe9\x94\x99\xe8\xaf\xaf"); // 错误
    }
    static inline QString kwFailed() { 
        return QString::fromUtf8("\xe5\xa4\xb1\xe8\xb4\xa5"); // 失败
    }
    static inline QString kwStart() { 
        return QString::fromUtf8("\xe5\xbc\x80\xe5\xa7\x8b"); // 开始
    }
    static inline QString kwProcessing() { 
        return QString::fromUtf8("\xe5\xa4\x84\xe7\x90\x86"); // 处理
    }
    static inline QString kwLoaded() { 
        return QString::fromUtf8("\xe5\xb7\xb2\xe5\x8a\xa0\xe8\xbd\xbd"); // 已加载
    }
    static inline QString kwSelected() { 
        return QString::fromUtf8("\xe5\xb7\xb2\xe9\x80\x89\xe6\x8b\xa9"); // 已选择
    }
    
    // MainWindow strings
    static inline QString cameraService() { 
        return QString::fromUtf8("\xe7\x9b\xb8\xe6\x9c\xba\xe6\x9c\x8d\xe5\x8a\xa1\xe7\xa8\x8b\xe5\xba\x8f"); // 相机服务程序
    }
    static inline QString navigationPage() { 
        return QString::fromUtf8("\xe5\xaf\xbc\xe8\x88\xaa\xe9\xa1\xb5\xe9\x9d\xa2"); // 导航页面
    }
    static inline QString contentPage() { 
        return QString::fromUtf8("\xe5\x86\x85\xe5\xae\xb9\xe9\xa1\xb5\xe9\x9d\xa2"); // 内容页面
    }
    static inline QString functions() { 
        return QString::fromUtf8(">> \xe5\x8a\x9f\xe8\x83\xbd"); // >> 功能
    }
    static inline QString plyTiffConverter() { 
        return QString::fromUtf8("   PLY-TIFF\xe8\xbd\xac\xe6\x8d\xa2\xe5\x99\xa8"); // PLY-TIFF转换器
    }
    static inline QString others() { 
        return QString::fromUtf8(">> \xe5\x85\xb6\xe5\xae\x83"); // >> 其它
    }
};

#endif // UI_STRINGS_H
