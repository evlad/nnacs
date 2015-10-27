#!/bin/sh
cmake -DCMAKE_INSTALL_PREFIX:PATH=$HOME/nnacs-1.7b . && make all install
cp noc_labs/nnacs.desktop ~/.local/share/applications/nnacs.desktop
