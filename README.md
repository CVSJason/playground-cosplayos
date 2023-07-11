仿照 Haribote OS（《30天自制操作系统》）的屑作
使用 clang，gcc 和 nasm。

作者平时没时间维护，所以请不要fork（

## 如何编译

编译需要在 linux 环境下进行。

1. git clone（别告诉我不会clone）
2. 在项目根目录下新建 fs_root 和 temp 文件夹，在 temp 文件夹下分别新建 fs、graphics、hardware 和 programs 文件夹
3. 在项目根目录下运行 `make -f makefile`

## 许可

该项目采用 MIT 许可证（英文全文位于 license）。在 golibc 文件夹下的 .h 文件和 .a 文件，作者为川合秀实（H.Kawai），采用 KL-01 许可证。
