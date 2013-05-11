#!/bin/sh

if [ ! -d ../build/Headers/GLP ]; then
    echo "Create symbol link of header files..."
    mkdir -p ../build/Headers
    cd ../build/Headers
    ln -s ../../GLP/Classes GLP
    cd -
fi
