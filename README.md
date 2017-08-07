# Java Wrapper for Poprtfolio - Optimizer

## 依赖

* cmake >= 2.8.0
* JDK >= 1.8.0
* boost >= 1.58.0

## 安装

1. 设置Boost变量（windows）

    修改根目录下CMakeLists.txt如下行，使其指向BOOST的根目录：

    ```
    set(BOOST_ROOT D:/dev/boost_1_64_0)
    ```

2. 生成项目文件

    例如在windows下，生产VS2015的项目文件：

    ```
    mkdir build
    cd build
    cmake -G "Visual Studio 14 2015 Win64" ..
    ```

    这时在build目录下会有生产的visual studio solution文件。

## 直接使用build脚本

* windows

```bash
./build_windows.bat
```  
