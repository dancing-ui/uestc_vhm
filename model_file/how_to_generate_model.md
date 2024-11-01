# Yolo模型导出
[教程](https://blog.csdn.net/m0_72734364/article/details/128758544)
# Fast-Reid模型导出
1. 下载[fast-reid仓库](https://github.com/JDAI-CV/fast-reid)
```bash
cd /workspace
git clone git@github.com:JDAI-CV/fast-reid.git
```
2. 下载[pth预训练模型文件](https://github.com/JDAI-CV/fast-reid/blob/master/MODEL_ZOO.md)，把下载好的pth文件放到model目录下，假设名字为duke_sbs_R50-ibn.pth
```bash
mkdir -p /workspace/fast-reid/projects/FastRT/model
cp /workspace/duke_sbs_R50-ibn.pth /workspace/fast-reid/projects/FastRT/model
```
3. pth文件转wts文件，[教程](https://github.com/JDAI-CV/fast-reid/blob/master/projects/FastRT/tools/How_to_Generate.md)
   1. 装pytorch的容器
   ```bash
   cd /workspace/model_file/fast_reid/export/docker
   docker build -t fastreid:v1 .
   docker run --gpus all -it --name fast_reid -v /workspace/fast-reid:/workspace -d fastreid:v1
   ```
   2. 使用VSCode打开新创建的fast_reid容器，pth转wts文件
   ```bash
   cd /workspace
   python projects/FastRT/tools/gen_wts.py --config-file='configs/DukeMTMC/sbs_R50-ibn.yml' --verify --show_model --wts_path='projects/FastRT/model/duke_sbs_R50-ibn.wts' MODEL.WEIGHTS 'projects/FastRT/model/duke_sbs_R50-ibn.pth'  MODEL.DEVICE "cuda:0"
   ```
4. wts文件转engine文件
   1. 用VSCode重新进入uestc_vhm容器，你会发现/workspace/fast-reid/projects/FastRT/model路径下多了一个duke_sbs_R50-ibn.wts文件
   2. 现在我们需要使用FastRT提供的工具fastrt，把wts文件转engine文件。当然，uestc_vhm容器的环境已经符合fastrt工具的编译环境了，可以直接按照[教程](https://github.com/JDAI-CV/fast-reid/blob/master/projects/FastRT/README.md)编译即可

# Fast-Reid动态库编译
由于源码本身存在路径依赖问题，所以就不能把fast-reid的动态库编译安装写到Dockerfile里面了
## tensorrt路径问题
- 问题：由于仓库作者在fast-reid/projects/FastRT/fastrt/CMakeLists.txt文件中指明了tensorrt的位置为/usr/include/x86_64-linux-gnu/，而我们安装的tensorrt位置为/usr/local/TensorRT-8.6.1.6，所以直接编译会面临找不到tensorrt相关库的问题
- 解决方案：替换/usr/include/x86_64-linux-gnu/为/usr/local/TensorRT-8.6.1.6/include手动编译安装fast-reid动态库
  - 更换以下内容：
      ```cmake
      # tensorrt
      include_directories(/usr/local/TensorRT-8.6.1.6/include)
      link_directories(/usr/local/TensorRT-8.6.1.6/include)
      ......
      target_link_libraries(${PROJECT_NAME} 
      /usr/local/TensorRT-8.6.1.6/lib/libnvinfer.so
      cudart
      ${OpenCV_LIBS}
      )
      ``` 
## 接口更换问题
- 问题：由于仓库作者使用的tensorrt版本较老，导致某些接口存在问题，导致编译失败
- 解决方案：我这里的TensorRT-8.6.1.6遇到的接口问题是某些接口缺少noexcept关键字，在报错的地方全部加上noexcept关键字即可

## 编译安装命令
解决上述问题之后，顺利的话，运行下面的命令即可完全编译安装
```bash
git clone https://github.com/JDAI-CV/fast-reid.git \
&& cd "$(pwd)/fast-reid/projects/FastRT" \
&& cmake -S . -B build \
-DCMAKE_BUILD_TYPE=Release \
-DBUILD_FASTRT_ENGINE=ON \
-DBUILD_DEMO=OFF \
-DBUILD_FP16=ON \
-DCMAKE_INSTALL_PREFIX=/usr/local/fast_reid \
&& make -C build -j$(nproc) \
&& make -C build install \
&& echo "export LD_LIBRARY_PATH=\"/usr/local/fast_reid/lib:$LD_LIBRARY_PATH\"" >> ~/.zshrc
```