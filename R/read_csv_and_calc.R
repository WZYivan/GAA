library(Rcpp)

df <- read.csv("data/gths135a.18f.dat.csv", stringsAsFactors = FALSE)

row1 = df[1,]

print(row1)
print(typeof(row1))
print(colnames(df))

Sys.setenv(LD_LIBRARY_PATH = "/home/azusa/file/project/GAA/build/src")
dyn.load("R/src/gaa.so")
sv_pos_from_broadcast(row1, 1024)

Sys.setenv(PKG_LIBS = "-L/home/azusa/file/project/GAA/build/src -lgaa -Wl,-rpath=/home/azusa/file/project/GAA/build/src")
Sys.setenv(PKG_CPPFLAGS="-I/home/azusa/file/project/GAA/include -I/usr/local/lib/R/library/RInside/include -I/usr/local/lib/R/library/Rcpp/include -I/usr/local/include/eigen3")

Rcpp::sourceCpp("R/src/gaa.cpp")
