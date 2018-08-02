FROM ubuntu:16.04

# create terrama2 user
ENV USER=terrama2
RUN useradd -s /bin/bash -m ${USER}
RUN echo ${USER}:${USER} | chpasswd
RUN usermod -aG sudo ${USER}

# install minimal dependencies
RUN apt-get update && apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    git \
    make \
    sudo \
    wget \
    # add-apt-repository
    software-properties-common python-software-properties \
    --no-install-recommends

# GCC 8
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt update && \
    apt install -y gcc-8 g++-8 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100  && \
    update-alternatives --install /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-8 100  && \
    update-alternatives --install /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-8 100  && \
    update-alternatives --install /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-8 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 100

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

RUN apt install -y gdb valgrind cppcheck

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

RUN echo "${USER} ALL=(ALL) NOPASSWD: /usr/bin/apt-get" >> /etc/sudoers && \
    echo "${USER} ALL=(ALL) NOPASSWD: /usr/bin/update-alternatives" >> /etc/sudoers && \
    echo "${USER} ALL=(ALL) NOPASSWD: /usr/sbin/usermod" >> /etc/sudoers && \
    echo "${USER} ALL=(ALL) NOPASSWD: /usr/sbin/groupadd" >> /etc/sudoers

# clean apt
RUN rm -rf /var/lib/apt/lists/*

# 
RUN wget https://cmake.org/files/v3.11/cmake-3.11.1-Linux-x86_64.sh && \
    sh cmake-3.11.1-Linux-x86_64.sh --prefix=/usr/local --exclude-subdir

# change user
USER ${USER}

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

ENV DEVEL_DIR=/home/${USER}/devel
ENV SCRIPTS_DIR=/home/${USER}/scripts
ENV DEPENDENCIES_DIR=${DEVEL_DIR}/dependencies

ENV TERRALIB_DIR=${DEVEL_DIR}/terralib
ENV TERRAMA2_DIR=${DEVEL_DIR}/terrama2

RUN mkdir -p ${SCRIPTS_DIR}

# Create sistem link clang-6.0 -> clang
ADD ./clang_update-alternative.sh ${SCRIPTS_DIR}
RUN cd ${SCRIPTS_DIR} && \
    ./clang_update-alternative.sh

ADD clang-tidy-diff.py ${SCRIPTS_DIR}
ADD pre-commit ${SCRIPTS_DIR}
RUN cd ${SCRIPTS_DIR} && \
        sed -i \
        -e "s@\${SCRIPTS_DIR}@${SCRIPTS_DIR}@g" \
        pre-commit

ADD install-3rdparty-linux-ubuntu-16.04.sh ${SCRIPTS_DIR}

ADD prepare_terralib.sh ${SCRIPTS_DIR}
RUN cd ${SCRIPTS_DIR} && \
        sed -i \
        -e "s@\${DEPENDENCIES_DIR}@${DEPENDENCIES_DIR}@g" \
        -e "s@\${TERRALIB_DIR}@${TERRALIB_DIR}@g" \
        -e "s@\${SCRIPTS_DIR}@${SCRIPTS_DIR}@g" \
        prepare_terralib.sh

ADD prepare_terrama2.sh ${SCRIPTS_DIR}
RUN cd ${SCRIPTS_DIR} && \
        sed -i \
        -e "s@\${DEPENDENCIES_DIR}@${DEPENDENCIES_DIR}@g" \
        -e "s@\${TERRALIB_DIR}@${TERRALIB_DIR}@g" \
        -e "s@\${TERRAMA2_DIR}@${TERRAMA2_DIR}@g" \
        -e "s@\${SCRIPTS_DIR}@${SCRIPTS_DIR}@g" \
        prepare_terrama2.sh

WORKDIR /home/${USER}

ADD config_permission.sh .
CMD /home/terrama2/config_permission.sh; sleep infinity

