FROM ubuntu:16.04

# create terrama2 user
ENV TERRAMA2_USER=terrama2
RUN useradd -ms /bin/bash ${TERRAMA2_USER}
RUN echo ${TERRAMA2_USER}:${TERRAMA2_USER} | chpasswd
RUN usermod -aG sudo ${TERRAMA2_USER}

# install minimal dependencies
RUN apt-get update && apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    git \
    cmake-gui \
    sudo \
    wget \
    # add-apt-repository
    software-properties-common python-software-properties \
    --no-install-recommends

# GCC 8
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt update && \
    apt install -y gcc-8 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100  && \
    update-alternatives --install /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-8 100  && \
    update-alternatives --install /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-8 100  && \
    update-alternatives --install /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-8 100

# Clang 6
RUN sh -c 'echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-6.0 main" > /etc/apt/sources.list.d/llvm.list' && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    apt update && \
    apt install -y \
    clang-6.0 \
    clang-format-6.0 \
    clang-tidy-6.0 \
    clang-tools-6.0 \
    lldb-6.0 \
    python-clang-6.0 \
    python-lldb-6.0

# Create sistem link clang-6.0 -> clang
ADD ./clang_update-alternative.sh /home/${TERRAMA2_USER}/scripts/
RUN cd /home/${TERRAMA2_USER}/scripts/ && \
    chmod +x clang_update-alternative.sh && \
    ./clang_update-alternative.sh

RUN apt install -y gdb valgrind

# Install VSCode
RUN curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg && \
    mv microsoft.gpg /etc/apt/trusted.gpg.d/microsoft.gpg && \
    sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main" > /etc/apt/sources.list.d/vscode.list' && \
    apt-get update && \
    apt-get install -y code \
    libasound2 \
    libatk1.0-0 \
    libcairo2 \
    libcups2 \
    libexpat1 \
    libfontconfig1 \
    libfreetype6 \
    libgtk2.0-0 \
    libpango-1.0-0 \
    libx11-xcb1 \
    libxcomposite1 \
    libxcursor1 \
    libxdamage1 \
    libxext6 \
    libxfixes3 \
    libxi6 \
    libxrandr2 \
    libxrender1 \
    libxss1 \
    libxtst6 \
    --no-install-recommends

# RUN mkdir -p /opt/qtcreator && \
#     cd /opt/qtcreator && \
#     apt-get update && apt-get install -y libxcb-keysyms1-dev libxcb-image0-dev \
#     libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev \
#     libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev \
#     libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev \
#     libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev x11vnc \
#     xauth build-essential mesa-common-dev libglu1-mesa-dev libxkbcommon-dev \
#     libxcb-xkb-dev libxslt1-dev libgstreamer-plugins-base0.10-dev p7zip-full && \
#     wget https://download.qt.io/official_releases/qtcreator/4.6/4.6.1/installer_source/linux_gcc_64_rhel72/qtcreator.7z && \
#     7z x qtcreator.7z && \
#     cd /usr/local/bin && \
#     ln -s /opt/qtcreator/bin/qtcreator

# clean apt
RUN rm -rf /var/lib/apt/lists/*

# change user
USER ${TERRAMA2_USER}

# install vscode extensions !!! AS USER !!!
RUN code --install-extension ms-vscode.cpptools && \
    code --install-extension twxs.cmake && \
    code --install-extension vector-of-bool.cmake-tools && \
    code --install-extension vadimcn.vscode-lldb && \
    code --install-extension bbenoist.doxygen && \
    code --install-extension cschlosser.doxdocgen && \
    code --install-extension ajshort.include-autocomplete && \
    code --install-extension reloadedextensions.reloaded-cpp && \
    code --install-extension reloadedextensions.reloaded-themes

WORKDIR /home/${TERRAMA2_USER}
CMD bash 
