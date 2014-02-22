#!/bin/sh
cmake -DCMAKE_INSTALL_PREFIX:PATH=$HOME/nnacs-1.6 . && make all install
