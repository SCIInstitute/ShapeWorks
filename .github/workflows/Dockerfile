# Based on manylinux_2_28 (AlmaLinux 8, GLIBC 2.28) for maximum HPC compatibility
# Binaries built here will run on RHEL/Rocky/Alma 8+, Ubuntu 20.04+, and most HPC clusters

# To build (e.g. for ShapeWorks 6.7):
#    docker build --progress=plain -t akenmorris/manylinux-build-box-sw67 .
# To publish:
#    docker push akenmorris/manylinux-build-box-sw67

FROM quay.io/pypa/manylinux_2_28_x86_64 AS env
MAINTAINER akenmorris@gmail.com

# Set environment variables
ENV PATH=/opt/rh/gcc-toolset-12/root/usr/bin:/opt/conda/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Enable PowerTools repo for additional development packages
RUN dnf install -y dnf-plugins-core && dnf config-manager --set-enabled powertools

# Update and install build tools
# gcc-toolset-12 provides modern GCC 12 while keeping GLIBC 2.28 for binary compatibility.
# The conda Qt5/ICU packages require GLIBCXX_3.4.28+ which system GCC 8 doesn't provide.
RUN dnf update -y && dnf groupinstall -y "Development Tools" \
    && dnf install -y gcc-toolset-12-gcc gcc-toolset-12-gcc-c++ gcc-toolset-12-binutils

# Ensure gcc-toolset-12's libstdc++ is available at link time
ENV LD_LIBRARY_PATH=/opt/rh/gcc-toolset-12/root/usr/lib64:/opt/rh/gcc-toolset-12/root/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

# Install git and git-lfs
RUN dnf install -y git curl
RUN curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.rpm.sh | bash
RUN dnf install -y git-lfs

# Install other dependencies
RUN dnf install -y rsync freeglut-devel mesa-libGL-devel mesa-libEGL-devel zip cups-libs pigz wget ccache

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
