
all: conv2d_3x3 fft fir gemm md-knn pedometer stencil2d

clean-all: clean-testaffine clean-conv2d_3x3 clean-fft clean-fir clean-gemm clean-md-knn clean-pedometer clean-stencil2d


# application: conv2d_3x3
.PHONY : conv2d_3x3
conv2d_3x3:
	@echo "============================ conv2d_3x3 =================================="
	cd ./conv2d_3x3; bash ../compile.sh convolution2d convolution2d
	@echo "============================ conv2d_3x3 =================================="
	@echo
	@echo

clean-conv2d_3x3:
	cd ./conv2d_3x3; rm -f ./*.ll ./*.dot

# application: fft
.PHONY : fft
fft:
	@echo "============================ fft =================================="
	cd ./fft; bash ../compile.sh fft fft4
	@echo "============================ fft =================================="
	@echo
	@echo

clean-fft:
	cd ./fft; rm -f ./*.ll ./*.dot

# application: fir
.PHONY : fir
fir:
	@echo "============================ fir =================================="
	cd ./fir; bash ../compile.sh kernel kernel
	@echo "============================ fir =================================="
	@echo
	@echo

clean-fir:
	cd ./fir; rm -f ./*.ll ./*.dot
	
# application: gemm
.PHONY : gemm
gemm:
	@echo "============================ gemm =================================="
	cd ./gemm; bash ../compile.sh gemm gemm
	@echo "============================ gemm =================================="
	@echo
	@echo

clean-gemm:
	cd ./gemm; rm -f ./*.ll ./*.dot
	
# application: md-knn
.PHONY : md-knn
md-knn:
	@echo "============================ md-knn =================================="
	cd ./md-knn; bash ../compile.sh md md_kernel
	@echo "============================ md-knn =================================="
	@echo
	@echo

clean-md-knn:
	cd ./md-knn; rm -f ./*.ll ./*.dot

# application: pedometer
.PHONY : pedometer
pedometer:
	@echo "============================ pedometer =================================="
	cd ./pedometer; bash ../compile.sh pedometer pedometer
	@echo "============================ pedometer =================================="
	@echo
	@echo

clean-pedometer:
	cd ./pedometer; rm -f ./*.ll ./*.dot
	
# application: stencil2d
.PHONY : stencil2d
stencil2d:
	@echo "============================ stencil2d =================================="
	cd ./stencil2d; bash ../compile.sh stencil stencil
	@echo "============================ stencil2d =================================="
	@echo
	@echo

clean-stencil2d:
	cd ./stencil2d; rm -f ./*.ll ./*.dot
