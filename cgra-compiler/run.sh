# SPDLOG_LEVEL=trace, debug, info, warn, err, critical, off
./build/cgra-compiler SPDLOG_LEVEL=off \
	-c true -m true -o true -t 3600000 -i 20000 \
	-p "../cgra-mg/src/main/resources/operations.json" \
	-a "../cgra-mg/src/main/resources/cgra_adg.json" \
	-d "../benchmarks/test4/fft/affine.json"
	# -d "../benchmarks/test4/conv2d_3x3/affine.json ../benchmarks/test4/fir/affine.json ../benchmarks/test4/pedometer/affine.json ../benchmarks/test4/stencil2d/affine.json ../benchmarks/test4/gemm/affine.json ../benchmarks/test4/fft/affine.json ../benchmarks/test4/md-knn/affine.json"
	# -d "../benchmarks/test4/testaffine/affine.json" 
    # -d "../benchmarks/test4/conv2d_3x3/affine.json"
    # -d "../benchmarks/test4/fir/affine.json"
    # -d "../benchmarks/test4/pedometer/affine.json"
    # -d "../benchmarks/test4/stencil2d/affine.json"   
    # -d "../benchmarks/test4/gemm/affine.json"
    # -d "../benchmarks/test4/fft/affine.json"
    # -d "../benchmarks/test4/md-knn/affine.json"  