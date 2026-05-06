ROOT = /home/azusa/file/project/GAA
	   
all: src/gaa.cpp
	PKG_CPPFLAGS="-I$(ROOT)/include -I/usr/local/lib/R/library/RInside/include -I/usr/local/lib/R/library/Rcpp/include -I/usr/local/include/eigen3" \
	PKG_LIBS="-L$(ROOT)/build/src -Wl,-rpath=$(ROOT)/build/src -lgaa" \
	R CMD SHLIB src/gaa.cpp