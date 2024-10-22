# cuda11.8-cudnn8-devel
FROM nvidia/cuda:11.8.0-cudnn8-devel-ubuntu20.04
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
    && apt-get -y install --no-install-recommends apt-utils build-essential git curl ca-certificates net-tools wget vim openssh-client cppcheck
# configure zsh
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends zsh \
    && chsh -s /bin/zsh \ 
    && sh -c "$(curl -fsSL https://install.ohmyz.sh/)" \
    && git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions \
    && git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting \
    && git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k \
    && sed -i 's/plugins=(git)/plugins=(git zsh-syntax-highlighting zsh-autosuggestions)/g' ~/.zshrc \
    && sed -i 's/ZSH_THEME="[^"]*"/ZSH_THEME="powerlevel10k\/powerlevel10k"/g' ~/.zshrc
# install clang
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends lsb-release wget software-properties-common gnupg \
    && wget https://apt.llvm.org/llvm.sh \
    && chmod u+x "$(pwd)/llvm.sh" \
    && "$(pwd)/llvm.sh" 18 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
    && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-18 100 \
    && apt-get install -y clang-format-18 clang-tidy-18 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100 \
    && rm "$(pwd)/llvm.sh"
# upgrade cmake 3.24.0
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libssl-dev \
    && wget "https://cmake.org/files/v3.24/cmake-3.24.0.tar.gz" \
    && tar -zxvf "$(pwd)/cmake-3.24.0.tar.gz" -C "/usr/local" \
    && rm "$(pwd)/cmake-3.24.0.tar.gz" \
    && cd "/usr/local/cmake-3.24.0" \
    && ./configure \
    && make -j$(nproc) \
    && make install \
    && update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 100 --force
# install faiss
RUN git clone https://github.com/xianyi/OpenBLAS.git \
    && cd "$(pwd)/OpenBLAS" \
    && make -j$(nproc) \
    && make PREFIX="/usr/local/openblas" install \
    && echo "export LD_LIBRARY_PATH=\"/usr/local/openblas/lib:$LD_LIBRARY_PATH\"" >> ~/.zshrc \
    && git clone https://github.com/facebookresearch/faiss.git \
    && cd "$(pwd)/faiss" \
    && cmake -S . -B build \
    -DFAISS_ENABLE_GPU=OFF \
    -DFAISS_ENABLE_PYTHON=OFF \
    -DFAISS_ENABLE_RAFT=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DFAISS_ENABLE_C_API=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DFAISS_USE_LTO=OFF \
    -DFAISS_ENABLE_ROCM=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local/faiss \
    && make -C build -j$(nproc) faiss \    
    && make -C build install \
    && echo "export LD_LIBRARY_PATH=\"/usr/local/faiss/lib:$LD_LIBRARY_PATH\"" >> ~/.zshrc

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
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd ../.. \
    && rm -rf opencv34
# install TensorRt
RUN wget https://developer.nvidia.com/downloads/compute/machine-learning/tensorrt/secure/8.6.1/tars/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz \
    && tar -zxvf "$(pwd)/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz" -C "/usr/local"\
    && rm "$(pwd)/TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-11.8.tar.gz" \
    && echo "export LD_LIBRARY_PATH=\"/usr/local/TensorRT-8.6.1.6/lib:$LD_LIBRARY_PATH\"" >> ~/.zshrc \
    && echo "export PATH=\"/usr/local/TensorRT-8.6.1.6/bin:$PATH\"" >> ~/.zshrc
# install ffmpeg
RUN apt-get -y install ffmpeg
# clean apt
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*