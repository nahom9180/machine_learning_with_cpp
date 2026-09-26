CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Icore -I.

# Binaries
TARGET   := app
TRAINER  := trainer
INIT     := model_init

# Core engine sources
CORE_SRCS := core/tokenizer.cpp core/embedding_store.cpp

.PHONY: all init train run clean

all: $(TARGET)

# Compile main app
$(TARGET): main.cpp $(CORE_SRCS)
	@mkdir -p output
	$(CXX) $(CXXFLAGS) main.cpp $(CORE_SRCS) -o $(TARGET)
	@echo "Build complete: ./$(TARGET)"

# Compile and run initializer
init: tools/model_init.cpp $(CORE_SRCS)
	@mkdir -p output
	$(CXX) $(CXXFLAGS) tools/model_init.cpp $(CORE_SRCS) -o $(INIT)
	./$(INIT)

# Compile trainer
train: trainer.cpp $(CORE_SRCS)
	@mkdir -p output
	$(CXX) $(CXXFLAGS) trainer.cpp $(CORE_SRCS) -o $(TRAINER)
	./$(TRAINER)

# Run main inference
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(INIT) $(TRAINER) output/*.tmp