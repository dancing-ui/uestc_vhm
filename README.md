# uestc_vhm
<div align="center">

  [![Cuda](https://img.shields.io/badge/CUDA-11.3-%2376B900?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit-archive)
  [![](https://img.shields.io/badge/TensorRT-8.6.1.6-%2376B900.svg?style=flat&logo=tensorrt)](https://developer.nvidia.com/nvidia-tensorrt-8x-download)
  [![](https://img.shields.io/badge/ubuntu-20.04-orange.svg?style=flat&logo=ubuntu)](https://releases.ubuntu.com/20.04/)

<br>
</div>

# 项目介绍
- 本项目的开发目标是为了快速部署目标检测、特征提取等算法模型，并将其通过流媒体的方式推流到前端页面进行显示。
- 本项目在开发过程中深入贯彻“高内聚、低耦合”的设计原则，力争很轻松地对目标检测、目标跟踪等算法和技术进行扩展。
# 效果展示
![alt text](doc/image/app_test.gif)
# 技术路线 
- 本项目主要使用了yolo系列模型完成目标检测，然后使用fast-reid提取目标框的特征向量，最后使用deepsort算法完成目标跟踪。
# 支持特性
- 流媒体技术：目前，本项目可以使用RTMP对摄像头或者视频进行推拉流，后续考虑添加RTSP。
- 目标检测：支持yolov8n。
- 目标跟踪：支持deepsort算法。
# 开发环境
- CPU: 13th Gen Intel(R) Core(TM) i5-13500HX   2.50 GHz
- GPU: NVIDIA GeForce RTX 4060 Laptop GPU
- RAM: 32GB
- Architecture: x86_64
- Development editor: VSCode
- Compiler: Clang 18.1.8
# QuickStart
## 环境配置
- 本项目使用Dockerfile管理开发环境，环境中已自带所有可用的TensorRT、Cuda、Cudann、Opencv、ffmpeg等依赖，按理来说，无需下载多余的包，仅需下载docker-desktop，并使用项目中的Dockerfile构建镜像即可。建议使用VSCode进行开发，VSCode环境配置见[链接](https://zhuanlan.zhihu.com/p/715594507)，只需要看插件配置那里即可，把插件安装上就行。
```bash
# docker开发环境配置
docker build -t uestc_vhm:v0 /home/xx/uestc_vhm/docker/Dockerfile
docker run --gpus all -it --name uestc_vhm -v /home/xx/uestc_vhm:/workspace -d uestc_vhm:v0
# 使用命令检验容器uestc_vhm是否可以调用GPU，正常输出应该有你的显卡型号，如下图输出 RTX 4060 Laptop GPU
docker run --rm -it --gpus=all nvcr.io/nvidia/k8s/cuda-sample:nbody nbody -gpu -benchmark
```
![alt text](doc/image/gpu_test.png)
## 模型配置
- 本项目所使用模型均通过官方仓库本地编译得来，且由于本项目使用了git-lfs来管理模型文件和视频文件，因此初期阶段无需下载模型文件和测试视频，模型文件存放在model_file目录下，视频文件在test_video目录下。
- 可以通过修改src/etc/uestc_vhm_cfg.json配置文件的路径来自定义配置（流媒体配置、模型配置）。
## 项目编译
提供两种编译方式，一种是用VSCode的.vscode配置文件，一种是使用脚本文件。
```bash
# 脚本文件编译
chmod +x /workspace/local_build.sh
/workspace/local_build.sh
```

# 感谢
- 理论指导来源于[链接](https://blog.csdn.net/LuohenYJ/article/details/122491044)
- 本项目参考了以下开源项目：
  - [fast-reid](https://github.com/JDAI-CV/fast-reid)
  - [deepsort](https://github.com/linghu8812/yolov5_fastreid_deepsort_tensorrt)
  - [yolo](https://github.com/FeiYull/TensorRT-Alpha)
- [错误记录文档链接（附详细复现流程）](https://arvas2ztsq.feishu.cn/drive/folder/ErYgf1ynRl0ZsNdICxzc45eVnWe?from=from_copylink)