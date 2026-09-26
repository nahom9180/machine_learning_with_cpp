CXX       := g++
CXXFLAGS  := -std=c++17 -O3 -Wall -Wextra

TARGET    := app
INIT      := model_init
SRCS      := main.cpp embedding_store.cpp tokenizer.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p output
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

init: model_init.cpp
	$(CXX) $(CXXFLAGS) model_init.cpp -o $(INIT)
	./$(INIT)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(INIT) output/*.bin output/*.tmp