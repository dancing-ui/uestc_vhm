# Yolo模型导出
[教程](https://blog.csdn.net/m0_72734364/article/details/128758544)
# Fast-reid模型导出
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
