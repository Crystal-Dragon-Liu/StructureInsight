# StructureInsight

StructureInsight是一个基于Qt的地质构造分析可视化工具，支持立体投影图和玫瑰图的绘制与分析。

## 功能特性

- **立体投影图(Stereonet)**：支持等面积和等角度投影，可显示平面和极点
- **玫瑰图(Rose Plot)**：支持走向/倾向数据的可视化，自动计算频率分布
- **数据管理**：支持从CSV文件读取地质数据
- **属性面板**：可自定义各种显示参数
- **自适应布局**：窗口大小调整时自动适应
- **交互控制**：支持不同类型图表的切换

## 系统要求

- Qt >= 6.10.1
- C++11兼容编译器
- 支持Windows、macOS、Linux平台

## 快速开始

1. **MacOS构建项目(其余待补充)**：
```bash
cmake -S . -B build/Qt_6_10_1_for_macOS-Debug
cmake --build build/Qt_6_10_1_for_macOS-Debug --target EVStereonet
```

2. **运行程序**：
```bash
./StructureInsight
```

3. **加载数据**：
   - 点击"..."按钮选择CSV数据文件
   - 点击"Load"按钮加载数据
   - 在左侧列表中选择数据类型

4. **切换图表类型**：
   - 使用底部的单选按钮切换立体投影图和玫瑰图

## 数据格式

CSV文件格式要求：
- 深度(DEPTH)：数值
- 走向(STRIKE)：0-360度
- 倾角类型(TYPE)：字符串
- 倾角值(DIP)：0-90度

示例：
```csv
DEPTH,STRIKE,TYPE,DIP
100,90,层理,45
200,180,裂缝,30
300,270,断层,60
```

## 开发说明

### 主要模块

- **StereonetWidget**：立体投影图绘制
- **RoseWidget**：玫瑰图绘制
- **AdvancedDipPane**：主界面容器
- **DipDataAccess**：数据读取和管理
- **PropertyPanel**：属性管理系统

### 扩展属性系统

属性系统支持以下类型：
- 整型(IntPropertyItem)
- 浮点型(FloatPropertyItem)
- 字符串(StringPropertyItem)
- 单选(SingleSelectPropertyItem)
- 多选(MultiSelectPropertyItem)

## 版本历史

- 0.0.1 (2026-04-04)：初始版本

## 许可证

MIT License