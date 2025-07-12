cmake_minimum_required(VERSION 3.10.0)
project(App VERSION 1.0.0)

# Устанавливаем путь к Homebrew
set(CMAKE_PREFIX_PATH "/opt/homebrew")

# Ищем GLFW
find_package(glfw3 REQUIRED)

# Ищем OpenGL
find_package(OpenGL REQUIRED)

# Ищем GLM
find_package(glm REQUIRED)

# Добавляем GLAD
add_library(glad dependencies/glad/src/glad.c)
target_include_directories(glad PUBLIC dependencies/glad/include)

add_executable(App src/config.h src/main.cpp)

target_include_directories(App PRIVATE dependencies)
target_include_directories(App PRIVATE dependencies/glad/include)

# Правильная линковка библиотек
target_link_libraries(App glfw)
target_link_libraries(App glad)
target_link_libraries(App ${OPENGL_LIBRARIES})

# Для macOS могут понадобиться дополнительные фреймворки
if(APPLE)
    target_link_libraries(App "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()