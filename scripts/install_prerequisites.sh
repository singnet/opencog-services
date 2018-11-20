#!/bin/bash

# It's adviseable to install C++ gRPC stuff only in the docker container
# because of this warning from their authors:
# 
# "WARNING: After installing with make install there is no easy way to uninstall,
# which can cause issues if you later want to remove the grpc and/or protobuf
# installation or upgrade to a newer version."

apt update
apt install -y git
apt install -y curl
apt install -y wget
apt install -y zip
apt install -y npm

# Not actually required but handy
apt install -y vim

export DEBIAN_FRONTEND=noninteractive
apt install -y tzdata
ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
dpkg-reconfigure --frontend noninteractive tzdata
export TMP_INSTALL_DIR=$1

# Python 3.6

# apt install -y python3
# apt install -y python3-pip
# 
################################################################################
# Manually install Python 3.6 in base images (e.g. Ubuntu 14.04) without the proper packages
apt install -y build-essential checkinstall zlib1g-dev libreadline-gplv2-dev libncursesw5-dev libssl-dev libsqlite3-dev tk-dev libgdbm-dev libc6-dev libbz2-dev
wget https://www.python.org/ftp/python/3.6.5/Python-3.6.5.tar.xz
tar xvf Python-3.6.5.tar.xz
cd Python-3.6.5/
./configure
make -j8
make install
cd .. ; rm -rf Python-3.6.5/; rm -f Python-3.6.5.tar.xz
################################################################################

# C++ gRPC and Protocol Buffers v3
apt-get install -y build-essential autoconf libtool pkg-config
apt-get install -y libgflags-dev libgtest-dev
apt-get install -y clang libc++-dev
rm -rf grpc
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init
make
make install
apt install -y openjdk-8-jdk
echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
apt-get update 
apt-get install -y bazel
apt-get upgrade -y bazel
bazel build :all
make install
# Proto3
cd third_party/protobuf
make 
make install
cd ../../..
#cd /opt/snet
#curl -OL https://github.com/google/protobuf/releases/download/v3.4.0/protoc-3.4.0-linux-x86_64.zip
#unzip protoc-3.4.0-linux-x86_64.zip -d protoc3
#mv protoc3/bin/* /usr/local/bin/
#mv protoc3/include/* /usr/local/include/
rm -rf grpc

# NodeJS 8.x

apt install -y nodejs
apt install -y npm
################################################################################
# Manually install nodejs and npm in base images without the proper packages
#
#curl -sL https://deb.nodesource.com/setup_8.x | sh
#apt install -y nodejs
#npm install -g npm
################################################################################

# Go

apt install -y golang-1.10
mkdir -p /opt/snet/go
cd $TMP_INSTALL_DIR
export GOPATH="/opt/snet/go"
export PATH="$PATH:/opt/snet/go/bin:/usr/lib/go-1.10/bin"
go get -v -u github.com/golang/dep/cmd/dep
go get -v -u github.com/golang/protobuf/protoc-gen-go
go get -v -u golang.org/x/lint/golint

# SNET Daemon

################################################################################
# Use pre-compiled release
curl -OL https://github.com/singnet/snet-daemon/releases/download/v0.1.0/snetd-0.1.0.tar.gz
tar xzvf snetd-0.1.0.tar.gz 
cp snetd-linux-amd64 /usr/local/bin/snetd
################################################################################
# Build using last available version
#cd /opt/snet/go/src
#git clone https://github.com/singnet/snet-daemon.git
#cd snet-daemon
#./scripts/install
#cp blockchain/agent.go vendor/github.com/singnet/snet-daemon/blockchain/
#./scripts/build linux amd64
#cp build/snetd-linux-amd64 ../../bin/snetd
################################################################################

# CLI

pip3 install --upgrade pip
apt install -y libusb-1.0-0-dev libudev1 libudev-dev
mkdir -p /tmp/install
cd $TMP_INSTALL_DIR
git clone https://github.com/singnet/snet-cli
cd snet-cli
./scripts/blockchain install
pip3 install -e .

# Opencog services

apt install -y libcurl4-openssl-dev
cd $TMP_INSTALL_DIR
wget http://ftp.cn.debian.org/debian/pool/main/n/nlohmann-json/nlohmann-json-dev_2.1.1-1.1_all.deb
apt install -y ./nlohmann-json-dev_2.1.1-1.1_all.deb
