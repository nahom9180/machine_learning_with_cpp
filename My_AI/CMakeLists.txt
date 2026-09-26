cmake_minimum_required(VERSION 3.16)

project(My_AI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(My_AI
    CMakeLists.txt
        core/config.hpp
        core/embedding_store.cpp
        core/embedding_store.hpp
        core/helen.hpp
        core/math.hpp
        core/tokenizer.cpp
        core/tokenizer.hpp
        data/dataset.txt
        data/dataset1.txt
        data/dataset_cleaned.txt
        data/dataset_generated.txt
        data/raw.txt
        data/shakespear.txt
        main.cpp
        Makefile
        model_init



        test.cpp
        test_math.cpp
        test_save.cpp
        tools/clean_text.py
        tools/generate_dataset.py
        tools/model_init.cpp
        trainer
        trainer.cpp
        ds.py

)

include(GNUInstallDirs)
install(TARGETS My_AI
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

