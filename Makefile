.DEFAULT_GOAL = help

# >>> GAA dirs >>>
GAA_ROOT = $(PROJ)/GAA
GAA_INC = $(GAA_ROOT)/include
GAA_SRC = $(GAA_ROOT)/src
GAA_DEV = $(GAA_ROOT)/dev
GAA_SCR = $(GAA_ROOT)/scripts
GAA_BUILD = $(GAA_ROOT)/build
# <<< GAA dirs <<<

# >>> scripts >>>
CONFIGURE = $(GAA_SCR)/configure.sh
TOKEI = $(GAA_SCR)/tokei.sh
SHELL = zsh
CMAKE = cmake
MAKE = make
TOKEI = tokei
# <<< scripts <<<

# >>> building >>>
CMAKE_DEFS ?=
CMAKE_DEFS += CMAKE_BUILD_TYPE=Debug 		\
        	  CMAKE_CXX_COMPILER=g++ 		\
        	  CMAKE_C_COMPILER=gcc 			\
        	  CMAKE_INSTALL_PREFIX=/tmp 	\
        	  GAA_EXT_VIS=ON 				\
        	  GAA_DEV=ON  					\
        	  GAA_TEST=ON 					\
        	  GAA_DOCS=ON 					\
        	  GAA_MODULE=OFF 				\
        	  GAA_WITH_R=OFF 				\
        	  GAA_USE_PRIVATE_3RD=OFF
CMAKE_OPTIONS = $(foreach DEF, $(CMAKE_DEFS), -D$(DEF))
config:
	mkdir -p $(GAA_BUILD)
	@$(CMAKE) -S $(GAA_ROOT) -B $(GAA_BUILD) $(CMAKE_OPTIONS)
# <<< building <<<

# >>> stat >>>
stat:
	@$(TOKEI) $(GAA_ROOT)/CMakeLists.txt $(GAA_ROOT)/cmake $(GAA_SRC) $(GAA_INC) -t="C++,C++ Header,CMake"
# <<< stat <<<

# >>> init >>>
init:
	rm -f init.sh
	touch init.sh
# 	echo "menu() { $(MAKE) -f $(GAA_ROOT)/Makefile $$""@ }\n" > init.sh
	echo "menu() {" > init.sh
	echo "	$(MAKE) -f $(GAA_ROOT)/Makefile $$""@" >> init.sh
	echo "}" >> init.sh
# <<< init <<<

# >>> help >>>
help:
	@echo "targets:"
	@echo "- config"
	@echo "- init"
	@echo "- stat"
# <<< help <<<