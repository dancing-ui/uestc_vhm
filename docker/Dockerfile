# cuda11.3-cudnn8-devel
FROM nvidia/cuda:11.3.1-cudnn8-devel-ubuntu20.04
# https://github.com/NVIDIA/nvidia-docker/issues/1632
RUN if [ -f /etc/apt/sources.list.d/nvidia-ml.list ]; then \
        echo "Removing /etc/apt/sources.list.d/nvidia-ml.list"; \
        rm /etc/apt/sources.list.d/nvidia-ml.list; \
    fi && \
    if [ -f /etc/apt/sources.list.d/cuda.list ]; then \
        echo "Removing /etc/apt/sources.list.d/cuda.list"; \
        rm /etc/apt/sources.list.d/cuda.list; \
    fi
# replace source
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
    && sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list
# install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends apt-utils build-essential cmake git zsh curl ca-certificates net-tools wget vim openssh-client    
# configure zsh
RUN chsh -s /bin/zsh \ 
    && sh -c "$(curl -fsSL https://install.ohmyz.sh/)" \
    && git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions \
    && git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting \
    && git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k \
    && sed -i 's/plugins=(git)/plugins=(git zsh-syntax-highlighting zsh-autosuggestions)/g' ~/.zshrc \
    && sed -i 's/ZSH_THEME="[^"]*"/ZSH_THEME="powerlevel10k\/powerlevel10k"/g' ~/.zshrc
# install dependencies
RUN apt-get dist-upgrade -y && \
    apt-get install -y \
    software-properties-common \
    libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev \
    python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev \ 	
    libdc1394-22-dev libgl1-mesa-glx
RUN mkdir opencv34 && cd opencv34 && \
    git clone -b 3.4 https://github.com/opencv/opencv && \
    git clone -b 3.4 https://github.com/opencv/opencv_contrib && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local/opencv \
    -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DBUILD_opencv_xfeatures2d=OFF \
    -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv && \
    make -j12 && \
    make install && \
    ldconfig && \
    cd ../.. \
    && rm -rf opencv34
# install TensorRt
RUN wget https://developer.nvidia.com/downloads/compute/machine-learning/tensorrt/secure/8.6.1/tars/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz \
    && tar -zxvf "$(pwd)/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz" -C "/usr/local"\
    && rm "$(pwd)/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz" \
    && echo -e "export LD_LIBRARY_PATH=\"/usr/local/TensorRT-8.6.1.6/lib:$LD_LIBRARY_PATH\"" >> ~/.zshrc\
    && echo -e "export PATH=\"/usr/local/TensorRT-8.6.1.6/bin:$PATH\"" >> ~/.zshrc
# install ffmpeg
RUN apt-get -y install ffmpeg
# update clang
RUN wget https://apt.llvm.org/llvm.sh \
    && chmod u+x "$(pwd)/llvm.sh" \
    && "$(pwd)/llvm.sh" 18 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
    && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-18 100 \
    && apt-get install -y clang-format-18 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100 \
    && rm "$(pwd)/llvm.sh"
# clean apt
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*