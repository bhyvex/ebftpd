#!/bin/bash

git clone -b 26compat https://github.com/mongodb/mongo-cxx-driver.git mongo-cxx-driver
cd mongo-cxx-driver/
scons --use-system-boost --prefix=$PWD/../mongo-cxx-driver-build --ssl --full install-mongoclient
 