#!/bin/sh
cmake -DCMAKE_INSTALL_PREFIX:PATH=$HOME/nnacs-1.6 . && make all install
cp noc_labs/nnacs.desktop ~/.local/share/applications/nnacs.desktop
