#!/usr/bin/env python3
"""
将PNG图标转换为ICO格式（Windows可执行文件图标）
使用方法: python convert_to_ico.py
需要安装: pip install Pillow
"""
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("错误: 需要安装Pillow库")
    print("请运行: pip install Pillow")
    exit(1)

# 源文件路径
png_file = Path("resources/icons/app.png")
# 目标ICO文件路径
ico_file = Path("resources/icons/app.ico")

if not png_file.exists():
    print(f"错误: 找不到图标文件 {png_file}")
    exit(1)

# 打开PNG图片
img = Image.open(png_file)

# Windows常用图标尺寸
sizes = [16, 32, 48, 64, 128, 256]

# 创建包含多个尺寸的图标列表
icon_images = []
for size in sizes:
    resized = img.resize((size, size), Image.Resampling.LANCZOS)
    icon_images.append(resized)

# 保存为ICO格式
ico_file.parent.mkdir(parents=True, exist_ok=True)
icon_images[0].save(ico_file, format='ICO', sizes=[(s, s) for s in sizes])

print(f"? 成功转换: {png_file} -> {ico_file}")
print(f"  包含尺寸: {', '.join([f'{s}x{s}' for s in sizes])}")
print("\n现在可以重新编译项目，.exe文件将显示自定义图标。")

