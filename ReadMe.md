# ply转tiff

该工具可以用于线扫相机与面阵相机

输入为ply文件，输出为xyz点云tiff图像和gray灰度tiff图像。
线扫相机可以选择图像的width，以width决定height。
面阵相机则需要选择输出的tiff图像的width和height。


![本地图片](./images/ply-tiff.png "ply-tiff转换器")

## requirements


```
qt5
opencv
happly(header only)
```

## run

```
1. config 3th party lib in CMakelists.txt 
2. mkdir build && cd build
3. cmake ..
4. vs2019
```