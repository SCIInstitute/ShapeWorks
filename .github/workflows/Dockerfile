# Based on old ubuntu to create more compatible binaries

# To build (e.g. for ShapeWorks 6.7):
#    docker build --progress=plain -t akenmorris/ubuntu-build-box-jammy-sw67 .
# To publish:
#    docker push akenmorris/ubuntu-build-box-jammy-sw67

FROM ubuntu:jammy-20250819 AS env
MAINTAINER akenmorris@gmail.com

# Set environment variables
ENV PATH=/opt/conda/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Update
RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y && apt-get install build-essential software-properties-common -y && add-apt-repository ppa:ubuntu-toolchain-r/test -y && apt-get update -y

# Install git and git-lfs
RUN add-apt-repository ppa:git-core/ppa
RUN apt-get update
RUN apt-get install git curl -y
RUN curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | bash
RUN apt-get install git-lfs -y

# Install other dependencies
RUN apt-get install rsync freeglut3-dev libgl1-mesa-dev libegl1-mesa zip libcups2 pigz wget ccache -y

# Install conda
RUN curl https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -o /tmp/Miniconda3-latest-Linux-x86_64.sh \
    && bash /tmp/Miniconda3-latest-Linux-x86_64.sh -b -p /opt/conda \
    && ln -s /opt/conda/etc/profile.d/conda.sh /etc/profile.d/conda.sh \
    && /opt/conda/bin/conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main \
    && /opt/conda/bin/conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r \
    && conda update -n base -c defaults conda \
    && conda install pip \
    && echo ". /opt/conda/etc/profile.d/conda.sh" >> ~/.bashrc \
    && echo "conda activate base" >> ~/.bashrc

# Get and decompress linuxdeployqt, it's complicated to use fuse with docker due to the kernel module
RUN curl -L -o $HOME/linuxdeployqt.AppImage https://github.com/probonopd/linuxdeployqt/releases/download/5/linuxdeployqt-5-x86_64.AppImage && chmod +x $HOME/linuxdeployqt.AppImage ; cd $HOME ; ./linuxdeployqt.AppImage --appimage-extract
RUN ln -s /root/squashfs-root/usr/bin/linuxdeployqt /usr/bin

# Set compression for ccache
RUN mkdir /root/.ccache && echo "max_size = 500M\ncompression = true" > /root/.ccache/ccache.conf
