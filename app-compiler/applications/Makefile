
all: array_add fft gemm pedometer

clean-all: clean-array_add clean-fft clean-gemm clean-pedometer

# application: array_add
.PHONY : array_add
array_add:
	@echo "============================ array_add =================================="
	cd ./array_add; bash ../compile.sh array_add array_add
	@echo "============================ array_add =================================="
	@echo
	@echo

clean-array_add:
	cd ./array_add; rm -f ./*.ll ./*.dot

# application: fft
.PHONY : fft
fft:
	@echo "============================ fft =================================="
	cd ./fft; bash ../compile.sh fix_fft fix_fft
	@echo "============================ fft =================================="
	@echo
	@echo

clean-fft:
	cd ./fft; rm -f ./*.ll ./*.dot

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

# application: pedometer
.PHONY : pedometer
pedometer:
	@echo "============================ pedometer =================================="
	cd ./pedometer; bash ../compile.sh adapt_pedometer_step_is_array pedometer
	@echo "============================ pedometer =================================="
	@echo
	@echo

clean-pedometer:
	cd ./pedometer; rm -f ./*.ll ./*.dot
