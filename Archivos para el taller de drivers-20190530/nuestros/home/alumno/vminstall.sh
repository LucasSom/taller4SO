#!/bin/bash


TOOLS="nasm gdb ddd openssh-server valgrind wget vim subversion nano openmpi-bin mpich2* dkms"

echo "Update... "
apt-get -y update
echo "Upgrade... "
apt-get -y upgrade
echo "Instala todo... "
apt-get -y install $TOOLS
echo "Terminado"


