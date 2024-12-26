#!/bin/bash

set -ex

sudo apt install --no-install-recommends -y curl xz-utils git aria2 \
  make \
  gcc \
  libseccomp-dev \
  libcap-dev \
  libc6-dev \
  binutils qemu-user-static

git clone https://github.com/dpkg123/ruri 1
cd 1
cc -Wl,--gc-sections -ftree-vectorize -flto -funroll-loops -finline-functions -march=native -mtune=native -static src/*.c src/easteregg/*.c -o ruri -lcap -lseccomp -lpthread -O3 -Wno-error
strip ruri
cp -v ruri /usr/local/bin/
ruri -v
cd ..
#BASE_URL="https://dl-cdn.alpinelinux.org/alpine/edge/releases/aarch64"
#ROOTFS_URL=$(curl -s -L "$BASE_URL/latest-releases.yaml" | grep "alpine-minirootfs" | grep "aarch64.tar.gz" | head -n 1 | awk '{print $2}')
#FULL_URL="$BASE_URL/$ROOTFS_URL"
#wget "$FULL_URL"
mkdir aarch64
#tar -xvf "$ROOTFS_URL" -C aarch64
#sudo debootstrap --arch=arm64 --variant=minbase bookworm aarch64
aria2c https://github.com/2cd/debian-museum/releases/download/12/12_bookworm_arm64.tar.zst -o root.tar.zst
tar -xf root.tar.zst -C aarch64
sudo rm aarch64/etc/resolv.conf
sudo echo nameserver 8.8.8.8 >aarch64/etc/resolv.conf
sudo cat | sudo tee -a aarch64/build.sh <<EOF
#!/bin/bash
set -x
apt update
apt install dpkg-dev git upx binutils -y
git clone https://github.com/dpkg123/ruri --depth=1
cd ruri
apt build-dep . -y
dpkg-buildpackage -b -us -uc -d
EOF
sudo chmod +x aarch64/build.sh
sudo ruri -a aarch64 -q /usr/bin/qemu-aarch64-static ./aarch64 /bin/sh /build.sh
sudo mv aarch64/*.deb ..
