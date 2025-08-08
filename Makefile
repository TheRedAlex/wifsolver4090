# GPL-3.0 © оригинальные авторы WifSolverCuda; адаптация под Ada (SM 8.9)
# Tested with CUDA 12.x/11.8 on Linux x86_64

# ——— Настройки устройства ———
COMPUTE_CAP        ?= 89          # Ada Lovelace (RTX 4090)
BLOCKS_DEFAULT     ?= 1024        # ≈ 8× SM (4090 ~128 SM)
THREADS_DEFAULT    ?= 640         # оптимум при ~48 рег/тред (см. README)
STEPS_DEFAULT      ?= 5000

# ——— Инструменты ———
NVCC               ?= nvcc
CXX                ?= g++

# ——— Флаги компиляции ———
GENCODE            := -gencode arch=compute_$(COMPUTE_CAP),code=sm_$(COMPUTE_CAP)
FAST_MATH          := --use_fast_math
PTXAS              := -Xptxas=-v
HOSTCXX            := -Xcompiler "-fPIC -O3 -fno-exceptions -fno-rtti"
WARN               := -Xcompiler "-Wno-unused-parameter -Wno-unknown-pragmas"
LINEINFO           := -lineinfo

# При желании попробуйте ограничение регистров:
# REGCAP          := -maxrregcount=64

# ——— Файлы проекта ———
# Оставляем оригинальную структуру исходников репозитория без изменений.
SRCDIR             := .
TARGET             := WifSolverCuda

# Список .cu/.cpp возьмётся из каталога автоматически
CU_SRCS            := $(wildcard $(SRCDIR)/*.cu)
CPP_SRCS           := $(wildcard $(SRCDIR)/*.cpp)
OBJS               := $(CU_SRCS:.cu=.o) $(CPP_SRCS:.cpp=.o)

# ——— Правила ———
all: $(TARGET)

$(TARGET): $(OBJS)
	$(NVCC) $(GENCODE) $(FAST_MATH) $(PTXAS) $(LINEINFO) $(HOSTCXX) $(WARN) \
	-o $@ $^ $(REGCAP)

%.o: %.cu
	$(NVCC) -O3 -std=c++17 $(GENCODE) $(FAST_MATH) $(PTXAS) $(LINEINFO) \
	$(HOSTCXX) $(WARN) $(REGCAP) -c $< -o $@

%.o: %.cpp
	$(CXX) -O3 -std=c++17 -fPIC -fno-exceptions -fno-rtti -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Подсказка по дефолтным тюнинговым флагам запуска (см. README):
print-defaults:
	@echo "-b $(BLOCKS_DEFAULT) -t $(THREADS_DEFAULT) -s $(STEPS_DEFAULT)"

.PHONY: all clean print-defaults
