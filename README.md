# Geomatics Algorithm Application

Implementation of algorithms widely used in geomatics, for undergraduate level.

## Contents

### GNSS
- Cycle slip editing

### Geodesy
- Gauss-Kruger projection
- Vincenty formula solution
- Bessel solution
- Gauss lemma solution
- Map index

### Photogrammetry
- Space resection
- Space intersection

## Build

### GNU/Linux

Here it's so easy, just as usual
```shell
mkdir -p build && cd build
cmake ..
make -j16
```

### Windows

This project provide a All-In-One build option for windows user, so configure and build as below
```shell
mkdir build
cd build
cmake .. -DGAA_USE_PRIVATE_3RD=ON
cmake --build .
```

## Tips
- Just treat `dev/` folder as some examples