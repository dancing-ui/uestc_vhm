# uestc_vhm
<div align="center">

  [![Cuda](https://img.shields.io/badge/CUDA-11.3-%2376B900?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit-archive)
  [![](https://img.shields.io/badge/TensorRT-8.6.1.6-%2376B900.svg?style=flat&logo=tensorrt)](https://developer.nvidia.com/nvidia-tensorrt-8x-download)
  [![](https://img.shields.io/badge/windows-11-blue.svg?style=flat&logo=windows)](https://www.microsoft.com/)
  [![](https://img.shields.io/badge/ubuntu-22.04-orange.svg?style=flat&logo=ubuntu)](https://releases.ubuntu.com/22.04/)
  [![GitHub stars](https://img.shields.io/github/stars/dancing-ui/uestc_vhm.svg?style=flat-square&logo=github&label=Stars&logoColor=white)](https://github.com/dancing-ui/uestc_vhm)
<br>
</div>

# 项目介绍
- 本项目的开发目标是为了快速部署目标检测、特征提取等算法模型，并将其通过流媒体的方式推流到前端页面进行显示。
- 本项目在开发过程中深入贯彻“高内聚、低耦合”的设计原则，力争很轻松地对目标检测、目标跟踪等算法和技术进行扩展。
# 效果展示
<div align=center>
	<img src="doc/image/app_test.gif"/>
</div>

# 技术路线 
- 本项目主要使用了yolo系列模型完成目标检测，然后使用fast-reid提取目标框的特征向量，最后使用deepsort算法完成目标跟踪。
# 支持特性
- 流媒体技术：目前，本项目可以使用RTMP对摄像头或者视频进行推拉流，后续考虑添加RTSP以支持实时监控功能。
- 目标检测：支持yolov8n。
- 目标跟踪：支持deepsort算法，后续考虑添加bytetrack算法。
# 开发环境
- CPU: 13th Gen Intel(R) Core(TM) i5-13500HX   2.50 GHz
- GPU: NVIDIA GeForce RTX 4060 Laptop GPU
- RAM: 32GB
- OS platform: WSL 2.0(Ubuntu 22.04.3 LTS)
- Docker Development Environment: Ubuntu 20.04
- Architecture: x86_64
- Development editor: VSCode
- Compiler: Clang 18.1.8
# Quick Start
## 前置安装
1. 安装nvidia驱动，[安装教程](https://io.web3miner.io/worker-guides/install_with_windows/windows-an-zhuang-nvidia-qu-dong-cheng-xu)
2. 安装WSL2.0，并在WSL2.0中安装Ubuntu 22.04.3 LTS，[安装教程](https://learn.microsoft.com/zh-cn/windows/wsl/install-manual)
3. 安装docker-desktop
4. 在powershell中使用如下命令检验WSL安装情况，下图中的Ubuntu-22.04-UESTC是我自己取的名字
<div align=center>
	<img src="doc/image/wsl.png"/>
</div>

## 环境检查
- 检验docker环境
  - 本项目使用Dockerfile管理开发环境，环境中已自带所有可用的TensorRT、Cuda、Cudann、Opencv、Ffmpeg等依赖。按理来说，无需下载多余的包，仅需下载安装docker-desktop保证docker环境，并使用项目中的Dockerfile构建镜像即可。
  - 由于WSL使用的是docker-desktop，所以无需在Ubuntu 22.04.3 LTS安装docker，只需要保证docker-desktop在后台运行即可
  - 进入安装好的Ubuntu 22.04.3 LTS，使用以下命令在WSL中检验当前安装的docker-desktop是否可以调用GPU，，如果调用不了，尝试更新docker-desktop版本至v4.34.2及以上，或查看[文档](https://arvas2ztsq.feishu.cn/docx/KdMLdL3oyozvksxS7jgcOOi2nUs?from=from_copylink)寻找解决方案，也可利用后记中的联系方式与我取得联系，我会尽力帮助解决问题。
```bash
# 检验nvidia驱动是否正常，这里右上角输出的CUDA Version必须大于等于11.3
nvidia-smi
# 检验docker是否可以调用GPU
docker run --rm -it --gpus=all nvcr.io/nvidia/k8s/cuda-sample:nbody nbody -gpu -benchmark
```
    - 正常输出应该有你的显卡型号，如下图输出 RTX 4060 Laptop GPU
<div align=center>
	<img src="doc/image/gpu_test.png"/>
</div>

## 项目下载
- git相关文档
  - 安装与配置：https://zhuanlan.zhihu.com/p/512099806
  - 常用命令：https://zhuanlan.zhihu.com/p/678347984
```bash
# https
git clone https://github.com/dancing-ui/uestc_vhm.git
# or ssh
git clone git@github.com:dancing-ui/uestc_vhm.git
```
## 环境配置
- 构建镜像：使用以下命令构建docker镜像，该镜像占硬盘空间大约15GB，请确保硬盘空间足够。
```bash
# docker开发环境配置
cd /home/xy/work/uestc_vhm/docker/Dockerfile
docker build -t uestc_vhm:v0 .
docker run --gpus all -it --name uestc_vhm -v /home/xy/work/uestc_vhm:/workspace -d uestc_vhm:v0
```
- 打开容器：使用VSCode的docker插件，进入docker容器中进行开发，VSCode环境配置见[链接](https://zhuanlan.zhihu.com/p/715594507)，只需要看插件配置那里，把插件安装上即可。
- zsh配置：初次进入容器，会进入zsh配置界面，无需惊慌，按照屏幕提示选择即可。

- 检验TensorRT是否正常
```bash
cd /usr/local/TensorRT-8.6.1.6/samples/sampleOnnxMNIST
make -j20
cd ../../bin
./sample_onnx_mnist
```
    - 正常截图如下图，有PASSED TensorRT.sample_onnx_mnist输出
<div align=center>
	<img src="doc/image/tensorrt_test.png"/>
</div>

## 模型配置
- 本项目所使用模型均通过官方仓库本地编译得来，且由于本项目使用了git-lfs来管理模型文件和视频文件，因此初期阶段（若使用不同款显卡需要重新导出trt模型）无需下载模型文件和测试视频，模型文件存放在model_file目录下，视频文件在test_video目录下。
- 可以通过修改src/etc/uestc_vhm_cfg.json配置文件的路径来自定义配置（流媒体配置、模型配置）。
## 项目编译
- 提供两种编译方式，一种是用VSCode的.vscode配置文件，一种是使用脚本文件。
```bash
# 脚本文件编译
cd /workspace
chmod +x ./local_build.sh
./local_build.sh
```
## 项目运行
- 使用以下命令启动项目
```bash
cd /workspace/build/src
./uestc_vhm --config=/workspace/src/etc/uestc_vhm_cfg.json
```
## 运行时资源占用
<div align=center>
	<img src="doc/image/runtime.png"/>
</div>

# 感谢
- [理论指导](https://blog.csdn.net/LuohenYJ/article/details/122491044)
- 本项目参考了以下开源项目：
  - [fast-reid](https://github.com/JDAI-CV/fast-reid)
  - [deepsort](https://github.com/linghu8812/yolov5_fastreid_deepsort_tensorrt)
  - [yolo](https://github.com/FeiYull/TensorRT-Alpha)
- [错误记录文档链接（附详细开发流程）](https://arvas2ztsq.feishu.cn/drive/folder/ErYgf1ynRl0ZsNdICxzc45eVnWe?from=from_copylink)
## 程序退出
```bash
1. 使用ctrl+c退出uestc_vhm程序
2. 使用 kill -2 uestc_vhm_pid 命令退出uestc_vhm程序

ps -e|grep uestc_vhm|awk '{print $1}'|xargs kill -2
```
# 后记
- 有任何问题都可以联系我，欢迎大家提issues和pr
- 联系方式：
  - Email: fengx02@163.com
  - QQ: 2779856074
- 如果该项目有帮助到您，请点一下右上角的小星星，感谢您的支持 :)