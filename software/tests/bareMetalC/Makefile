include $(abs_top_srcdir)/Makefrag

tests = \
	conv2d_3x3 \
	fir \
	gemm_ncube \
	pedometer \
	stencil2d \
	knn \
	fft \
	pipeline \
	conv3x3_fir_unroll \
	pedometer_knn \
	stencil2d_gemm \
	parallel_pipeline
#
#
#
#
#	fir \
#	gemm_ncube \
#	gemver \
#	mvt \
#	pedometer \
#	stencil2d \
#	stencil3d \
#	multi_pipeline \
#	multi_pipeline_long \
#	ellpack \
#	knn

tests_baremetal = $(tests:=-baremetal)

#ifeq ($(findstring spike,$(RUNNER)),spike)
## Currently don't support conv or conv-with-pool on spike
#runs_baremetal = $(addsuffix .run,$(filter-out conv-baremetal conv_with_pool-baremetal,$(tests_baremetal)))
#else
## Don't run very long benchmarks for RTL sim
#runs_baremetal = $(addsuffix .run,$(filter-out tiled_matmul_cpu-baremetal tiled_matmul_option-baremetal,$(tests_baremetal)))
#endif

ifdef BAREMETAL_ONLY
	tests_linux =
	tests_pk =
else
	tests_linux = $(tests:=-linux)
	tests_pk = $(tests:=-pk)
endif


BAREMETAL_COMMON = $(abs_top_srcdir)/riscv-tests/benchmarks/common
FGRA_HEADERS =
#FGRA_HEADERS = $(abs_top_srcdir)/common/include/fgra.h $(abs_top_srcdir)/include/fgra_params.h

CFLAGS := $(CFLAGS) \
	-DPREALLOCATE=1 \
	-DMULTITHREAD=1 \
	-mcmodel=medany \
	-std=gnu99 \
	-O2 \
	-ffast-math \
	-fno-common \
	-fno-builtin-printf \
	-march=rv64gc -Wa,-march=rv64gcxhwacha \
	-lm \
	-lgcc \
	-I$(abs_top_srcdir)/riscv-tests \
	-I$(abs_top_srcdir)/riscv-tests/env \
	-I$(abs_top_srcdir) \
	-I$(BAREMETAL_COMMON) \
	-DID_STRING=$(ID_STRING) \

CFLAGS_PK := \
	$(CFLAGS) \
	-static \
	-DBAREMETAL=1 \

CFLAGS_BAREMETAL := \
	$(CFLAGS) \
	-nostdlib \
	-nostartfiles \
	-static \
	-T $(BAREMETAL_COMMON)/test.ld \
	-DBAREMETAL=1 \

all: $(tests_baremetal) $(tests_linux) $(tests_pk)

vpath %.c $(src_dir)

%-baremetal: %.c $(FGRA_HEADERS)
	$(CC_BAREMETAL) $(CFLAGS_BAREMETAL) $< $(LFLAGS) -o $@ \
		$(wildcard $(BAREMETAL_COMMON)/*.c) $(wildcard $(BAREMETAL_COMMON)/*.S) $(LIBS)

%-linux: %.c $(FGRA_HEADERS)
	$(CC_LINUX) $(CFLAGS) $< $(LFLAGS) -o $@

%-pk: %.c $(FGRA_HEADERS)
	$(CC_LINUX) $(CFLAGS_PK) $< $(LFLAGS) -o $@

run-baremetal: $(runs_baremetal)

%-baremetal.run: %-baremetal
	$(RUNNER)$(abs_top_srcdir)/build/bareMetalC/$^

junk += $(tests_baremetal) $(tests_linux) $(tests_pk)

