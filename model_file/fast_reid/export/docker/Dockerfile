FROM pytorch/pytorch:1.11.0-cuda11.3-cudnn8-devel
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
    && apt-get -y install --no-install-recommends build-essential cmake clang git zsh curl ca-certificates net-tools wget vim
# configure zsh
RUN chsh -s /bin/zsh \ 
    && sh -c "$(curl -fsSL https://install.ohmyz.sh/)" \
    && git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions \
    && git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting \
    && git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k \
    && sed -i 's/plugins=(git)/plugins=(git zsh-syntax-highlighting zsh-autosuggestions)/g' ~/.zshrc \
    && sed -i 's/ZSH_THEME="[^"]*"/ZSH_THEME="powerlevel10k\/powerlevel10k"/g' ~/.zshrc
# install dependencies
RUN echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc \
    && pip install --user -i https://pypi.tuna.tsinghua.edu.cn/simple tensorboard opencv-python cython yacs termcolor scikit-learn tabulate gdown gpustat faiss-gpu ipdb h5py

# # install Anaconda.   
# RUN wget https://repo.anaconda.com/archive/Anaconda3-2021.04-Linux-x86_64.sh \
#     && chmod +x "$(pwd)/Anaconda3-2021.04-Linux-x86_64.sh" \
#     && /bin/zsh "$(pwd)/Anaconda3-2021.04-Linux-x86_64.sh" -b \
#     && export PATH="/root/anaconda3/bin:$PATH" \
#     && conda init zsh \
#     && echo -e "auto_activate_base: true\nchannels:\n  - defaults\nshow_channel_urls: true\ndefault_channels:\n  \
#     - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main\n  \
#     - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/r\n  \
#     - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/msys2\ncustom_channels:\n  \
#     conda-forge: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     msys2: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     bioconda: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     menpo: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     pytorch: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     pytorch-lts: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     simpleitk: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud\n  \
#     deepmodeling: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/" > ~/.condarc \
#     && rm "$(pwd)/Anaconda3-2021.04-Linux-x86_64.sh"

# ENV DEBIAN_FRONTEND noninteractive
# RUN apt-get update && apt-get install -y \
# 	python3-opencv ca-certificates python3-dev git wget sudo ninja-build
# RUN ln -sv /usr/bin/python3 /usr/bin/python

# # create a non-root user
# ARG USER_ID=1000
# RUN useradd -m --no-log-init --system  --uid ${USER_ID} appuser -g sudo
# RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
# USER appuser
# WORKDIR /home/appuser

# # https://github.com/facebookresearch/detectron2/issues/3933
# ENV PATH="/home/appuser/.local/bin:${PATH}"
# RUN wget https://bootstrap.pypa.io/pip/3.6/get-pip.py && \
# 	python3 get-pip.py --user && \
# 	rm get-pip.py

# # install dependencies
# # See https://pytorch.org/ for other options if you use a different version of CUDA
# RUN pip install --user tensorboard cmake   # cmake from apt-get is too old
# RUN pip install --user torch==1.6.0+cu101 torchvision==0.7.0+cu101 -f https://download.pytorch.org/whl/cu101/torch_stable.html
# RUN pip install --user -i https://pypi.tuna.tsinghua.edu.cn/simple tensorboard opencv-python==4.3.0.38 cython yacs termcolor scikit-learn tabulate gdown gpustat faiss-gpu ipdb h5py
