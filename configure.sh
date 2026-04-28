# mkdir -p build
# cd build
cmake .. \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=g++\
    -DCMAKE_C_COMPILER=gcc \
    -DGAA_DEV=ON \
    -DGAA_TEST=ON \
    -DGAA_MODULE=OFF \
    -DGAA_WITH_R=ON