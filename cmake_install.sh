#!/bin/sh
cmake -DCMAKE_INSTALL_PREFIX:PATH=_debug_ . && make all install
mkdir -p ~/.local/share/applications
install -t ~/.local/share/applications noc_labs/nnacs.desktop
