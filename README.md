# Laser safety controller

## Installation of simulator

In WSL:
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    gcc \
    git \
    libsdl2-dev \
    mesa-utils \
    libgl1-mesa-glx \
    x11-apps

https://sourceforge.net/projects/vcxsrv/

Clone https://github.com/tjhowse/lv_port_pc_vscode

make clean
make
LIBGL_ALWAYS_INDIRECT=1 DISPLAY=:0 ./build/bin/demo
