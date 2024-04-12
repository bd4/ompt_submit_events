.PHONY: all
all: libSubmitOMPT.so vec

libSubmitOMPT.so: ompt_submit.cpp
	icpx -I./include ompt_submit.cpp -shared -fPIC -o $@

vec: vec.cpp
	icpx -fopenmp -I./include $< -o $@

.PHONY: clean
clean:
	$(RM) vec libSubmitOMPT.so
