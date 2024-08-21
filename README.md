# uestc_vhm
## Introduction
uestc_vhm mainly contains streaming, hardware, and model processing functions.
## Purpose
The goal is to quickly deploy the target detection algorithm model and stream it to the front-end page for display.
## Development Environment
- CPU: 13th Gen Intel(R) Core(TM) i5-13500HX   2.50 GHz
- GPU: NVIDIA GeForce RTX 4060 Laptop GPU
- RAM: 32GB
- Architecture: x86_64
- Development Platform: WSL 2.0 Ubuntu 22.04.3 LTS
- Development editor: VSCode
## Dependency
- cuda 11.8.0
- cudnn 8.6.0 for cuda11.x
- TensorRT 8.5.3.1
- ffmpeg 4.2.2
## Efficiency
- model: yolov8n
- batch_size: 8
- total frames of video: 7500
- pre process time: 0.257808ms
- infer time = 1.93941ms
- post process time = 0.197888ms