# ELoader - 一个mod注入工具(从Xenobyte分离)
### 使用方法:
将你要注入的mod放在此目录

然后打开控制台定位到此目录

输入指令:
```bash
java -jar packer.jar 你mod的文件名称.jar
```
会在此目录生成一个classes.h的文件

将此文件放入lib文件夹，然后构建

会生成一个out目录

打开 out\build\ 并寻找ELoader.exe和eloader_dll.dll

将这两个文件放在同一文件夹然后打开ELoader.exe即可自动注入

提示: 程序会注入此电脑运行的所有java程序。当电脑没有启动任何java程序时，程序无显示
