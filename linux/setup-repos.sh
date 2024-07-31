#!/bin/bash
# Renesas R-Car CR7 Solution
#
# The package is provided as a mix of tarballs and patches on top of git repos.
# This script clones the required git repos, unpack tarballs, and apply patches.
# If you already have a repo (e.g. linux) for which you just want to fetch and
# checkout the package, make sure you have a clean tree and no branches with the
# $BRANCH name set below.
#set -e

# Default package dir is the current dir
if [[ -z "${PKG_RELEASE}" ]]; then
	PKG_RELEASE=.
fi
PKG_RELEASE=$(readlink -f $PKG_RELEASE)

# Default working dir is $PKG_RELEASE/work
if [[ -z "${WORK}" ]]; then
	WORK=./work
fi
WORK=$(readlink -f $WORK)
mkdir -p ${WORK}

# Submodules init and update
git submodule update --init --recursive

# Preparing repos for the Yocto 5.9
YOCTO_PKGS=${YOCTO_DIR}/packages
mkdir -p ${YOCTO_PKGS}
cd ${YOCTO_DIR}
echo; echo "Cloning and checkout the Yocto repos from git"; echo;
cd ${YOCTO_DIR}/../
echo; echo "Installing Software Packages into Yocto directory"; echo;
unzip "${YOCTO_BSP_5_9}.zip"
unzip "Gen3e_v590_Software.zip"
cp -r Software/* ${YOCTO_PKGS}
cd ${YOCTO_DIR}
git clone git://git.yoctoproject.org/poky
cd poky
git checkout -b tmp 74b22db6879b388d700f61e08cb3f239cf940d18
cd ${YOCTO_DIR}
git clone git://git.openembedded.org/meta-openembedded
cd meta-openembedded
git checkout -b tmp 814eec96c2a29172da57a425a3609f8b6fcc6afe
cd ${YOCTO_DIR}
git clone https://github.com/renesas-rcar/meta-renesas.git
cd meta-renesas
git checkout -b tmp ff58fb97c4f9dcaad20445320644ca0f40c599a4
cd ${YOCTO_DIR}/../
### Please set up YOCTO-BSP in setup-env.sh ###
echo; echo "Copying proprietary software into Yocto directory"; echo;
cd ${YOCTO_DIR}/meta-renesas
sh meta-rcar-gen3/docs/sample/copyscript/copy_proprietary_softwares.sh ${YOCTO_PKGS}
cd ${YOCTO_DIR}
echo; echo "copy bblayers.conf and local.conf"; echo;
source poky/oe-init-build-env build
if [ "$1" = "ebisu" ]; then
    cp ${YOCTO_DIR}/meta-renesas/meta-rcar-gen3/docs/sample/conf/ebisu/poky-gcc/mmp/*.conf ./conf/.
	cp ./conf/local-wayland.conf ./conf/local.conf
	echo "MACHINE_FEATURES_append = \" safe-rendering\"" >> ./conf/local.conf
else
    cp ${YOCTO_DIR}/meta-renesas/meta-rcar-gen3/docs/sample/conf/salvator-x/poky-gcc/mmp/*.conf ./conf/.
	cp ./conf/local-wayland.conf ./conf/local.conf
fi