# **************************************************************************** #
#                                   SETTINGS                                   #
# **************************************************************************** #

NAME        = libftpp.a
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++11 -I$(INC_DIR)

# Carpetas
SRC_DIR      = src
INC_DIR      = includes
BUILD_DIR    = build
BIN_DIR      = bin
EXAMPLES_DIR = examples

# ================= MÓDULOS ================= #
DATA_STRUCTURES = data_structures
DESIGN_PATTERNS = design_patterns
IOSTREAM        = iostreams
THREADING       = threading
NETWORK         = network
MATHEMATICS     = mathematics
BONUS           = bonus

# ===============================================
# SRCS por módulo
# ===============================================

# DATA STRUCTURES sources
SRC_DATA_STRUCTURES = $(SRC_DIR)/$(DATA_STRUCTURES)/data_buffer.cpp

SRC_DESIGN_PATTERNS = $(SRC_DIR)/$(DESIGN_PATTERNS)/memento.cpp 

# IOSTREAM sources
SRC_IOSTREAM = $(SRC_DIR)/$(IOSTREAM)/thread_safe_iostream.cpp

# THREADING sources
SRC_THREADING = \
   $(SRC_DIR)/$(THREADING)/thread_safe_queue.cpp \
 	$(SRC_DIR)/$(THREADING)/thread.cpp \
	$(SRC_DIR)/$(THREADING)/worker_pool.cpp \
	$(SRC_DIR)/$(THREADING)/persistent_worker.cpp

# NETWORK sources
SRC_NETWORK = \
	$(SRC_DIR)/$(NETWORK)/message.cpp \
	$(SRC_DIR)/$(NETWORK)/server.cpp \
	$(SRC_DIR)/$(NETWORK)/client.cpp

# MATHEMATICS sources
SRC_MATHEMATICS = 
# 	$(SRC_DIR)/$(MATHEMATICS)/ivector2.cpp \
#	$(SRC_DIR)/$(MATHEMATICS)/ivector3.cpp \
#	$(SRC_DIR)/$(MATHEMATICS)/random_2D_coordinate_generator.cpp \
#	$(SRC_DIR)/$(MATHEMATICS)/perlin_noise_2D.cpp

# BONUS sources - NUEVO
SRC_BONUS = \
	$(SRC_DIR)/$(BONUS)/timer.cpp \
	$(SRC_DIR)/$(BONUS)/chronometer.cpp \
	$(SRC_DIR)/$(BONUS)/application.cpp \
	$(SRC_DIR)/$(BONUS)/widget.cpp

# Concatenar todos los src
SRCS := $(SRC_DATA_STRUCTURES) $(SRC_DESIGN_PATTERNS) $(SRC_IOSTREAM) \
        $(SRC_THREADING) $(SRC_NETWORK) $(SRC_MATHEMATICS)

# Incluir bonuses si se solicita
BONUS_MODE ?= 0
ifeq ($(BONUS_MODE),1)
	SRCS += $(SRC_BONUS)
endif

# Objetos - CORREGIDO: usar wildcard para verificar que existen
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Verificar que los archivos fuente existen
SRCS_EXIST := $(wildcard $(SRCS))

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

# ----------------------------- LIBRARY -------------------------------------- #
$(NAME): $(BUILD_DIR) $(OBJS)
	@echo "🔨 Creando librería estática..."
	@ar rcs $(NAME) $(OBJS)
	@echo "✅ Librería compilada: $(NAME)"

# ---------------------------- BONUS TARGET ---------------------------------- #
bonus:
	@echo "🏆 Compilando con bonuses..."
	@$(MAKE) BONUS_MODE=1

# ---------------------------- OBJECTS --------------------------------------- #
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "[CXX] Compilando $< -> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ---------------------------- FOLDERS --------------------------------------- #
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# ---------------------------- TEST ------------------------------------------ #
# Uso: make test TEST_NAME=nombre_test
test: $(NAME) | $(BIN_DIR)
	@if [ -z "$(TEST_NAME)" ]; then \
		echo "Uso: make test TEST_NAME=nombre_test"; \
		exit 1; \
	fi
	@if [ ! -f $(EXAMPLES_DIR)/$(TEST_NAME).cpp ]; then \
		echo "Error: test no encontrado: $(EXAMPLES_DIR)/$(TEST_NAME).cpp"; \
		exit 1; \
	fi
	@echo "[TEST] Compilando test $(TEST_NAME).cpp -> $(BIN_DIR)/$(TEST_NAME)"
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/$(TEST_NAME).cpp $(NAME) -o $(BIN_DIR)/$(TEST_NAME)
	@echo "✅ Test compilado: $(BIN_DIR)/$(TEST_NAME)"
	@echo "🚀 Ejecutando test..."
	@./$(BIN_DIR)/$(TEST_NAME)

# ---------------------------- BONUS TESTS ----------------------------------- #
test_timer: bonus
	@echo "[TEST] Compilando test del Timer..."
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/timer_test.cpp $(NAME) -o $(BIN_DIR)/timer_test
	@echo "🚀 Ejecutando test del Timer..."
	@./$(BIN_DIR)/timer_test

test_chronometer: bonus
	@echo "[TEST] Compilando test del Chronometer..."
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/chronometer_test.cpp $(NAME) -o $(BIN_DIR)/chronometer_test
	@echo "🚀 Ejecutando test del Chronometer..."
	@./$(BIN_DIR)/chronometer_test

test_application: bonus
	@echo "[TEST] Compilando test de Application..."
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/application_test.cpp $(NAME) -o $(BIN_DIR)/application_test
	@echo "🚀 Ejecutando test de Application..."
	@./$(BIN_DIR)/application_test

test_bonus: test_timer test_chronometer test_application

# ---------------------------- INFO ------------------------------------------ #
info:
	@echo "=== INFORMACIÓN DEL PROYECTO ==="
	@echo "Librería: $(NAME)"
	@echo "Compilador: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Modo Bonus: $(BONUS_MODE)"
	@echo "Fuentes encontradas: $(words $(SRCS_EXIST)) de $(words $(SRCS)) archivos"
	@echo "Archivos fuente:"
	@for src in $(SRCS_EXIST); do echo "  ✓ $$src"; done
	@echo "Módulos activos:"
	@if [ -n "$(SRC_DATA_STRUCTURES)" ]; then echo "  ✓ Data Structures"; fi
	@if [ -n "$(filter-out ,$(SRC_DESIGN_PATTERNS))" ]; then echo "  ✓ Design Patterns"; fi
	@if [ -n "$(filter-out ,$(SRC_IOSTREAM))" ]; then echo "  ✓ IOStream"; fi
	@if [ -n "$(filter-out ,$(SRC_THREADING))" ]; then echo "  ✓ Thread"; fi
	@if [ -n "$(filter-out ,$(SRC_NETWORK))" ]; then echo "  ✓ Network"; fi
	@if [ -n "$(filter-out ,$(SRC_MATHEMATICS))" ]; then echo "  ✓ Mathematics"; fi
	@if [ "$(BONUS_MODE)" = "1" ]; then echo "  ✓ Bonus (Timer, Chronometer, Application, Widget)"; fi

# ---------------------------- CHECK ----------------------------------------- #
check:
	@echo "=== VERIFICANDO ESTRUCTURA ==="
	@echo "Directorios:"
	@test -d $(SRC_DIR) && echo "  ✓ $(SRC_DIR)" || echo "  ✗ $(SRC_DIR) - NO EXISTE"
	@test -d $(INC_DIR) && echo "  ✓ $(INC_DIR)" || echo "  ✗ $(INC_DIR) - NO EXISTE"
	@test -d $(SRC_DIR)/$(BONUS) && echo "  ✓ $(SRC_DIR)/$(BONUS)" || echo "  ✗ $(SRC_DIR)/$(BONUS) - NO EXISTE"
	@test -d $(INC_DIR)/$(BONUS) && echo "  ✓ $(INC_DIR)/$(BONUS)" || echo "  ✗ $(INC_DIR)/$(BONUS) - NO EXISTE"
	@echo "Archivos fuente requeridos:"
	@for src in $(SRCS); do \
		if [ -f "$$src" ]; then \
			echo "  ✓ $$src"; \
		else \
			echo "  ✗ $$src - NO ENCONTRADO"; \
		fi; \
	done

# ---------------------------- CLEAN ----------------------------------------- #
clean:
	@rm -rf $(BUILD_DIR)
	@echo "🧹 Objetos eliminados."

fclean: clean
	@rm -rf $(NAME) $(BIN_DIR)
	@echo "🗑️  Limpieza completa del proyecto."

re: fclean all

rebonus: fclean bonus

# ---------------------------- HELP ------------------------------------------ #
help:
	@echo "=== COMANDOS DISPONIBLES ==="
	@echo "make all           - Compila la librería"
	@echo "make bonus         - Compila la librería con los bonuses"
	@echo "make clean         - Elimina archivos objeto"
	@echo "make fclean        - Elimina todo lo generado"
	@echo "make re            - Recompila desde cero"
	@echo "make rebonus       - Recompila desde cero con bonuses"
	@echo "make info          - Muestra información del proyecto"
	@echo "make check         - Verifica la estructura de archivos"
	@echo "make test          - Compila y ejecuta test (uso: make test TEST_NAME=nombre_test)"
	@echo "make test_timer    - Ejecuta test del Timer"
	@echo "make test_chronometer - Ejecuta test del Chronometer"
	@echo "make test_application - Ejecuta test de Application"
	@echo "make test_bonus    - Ejecuta todos los tests de bonus"

.PHONY: all clean fclean re test bonus test_timer test_chronometer test_application test_bonus info check help rebonus