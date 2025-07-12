#include "config.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type) {
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open(filepath);
    while (std::getline(file, line)) {
        bufferedLines << line << "\n";
    }
    std::string shaderSource = bufferedLines.str();
    const char* shaderSrc = shaderSource.c_str();
    bufferedLines.str("");
    file.close();

    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &shaderSrc, NULL);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
        std::cout << "shader module compilation error:\n" << errorLog << std::endl;
    }

    return shaderModule;
}

unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath) {
    std::vector<unsigned int> modules;
    modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
    modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

    unsigned int shader = glCreateProgram();
    for (unsigned int shaderModule : modules) {
        glAttachShader(shader, shaderModule);
    }
    glLinkProgram(shader);

    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetProgramInfoLog(shader, 1024, NULL, errorLog);
        std::cout << "shader linking error:\n" << errorLog << std::endl;
    }

    for (unsigned int shaderModule : modules) {
        glDeleteShader(shaderModule);
    }

    return shader;

}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cout << "GLFW couldn't start" << std::endl;
        return -1;
    }
    
    // Создание окна
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Устанавливаем контекст OpenGL
    // Нужно, чтобы все функции применялись к этому окну 
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      glfwTerminate();
      return -1;
    }

    glClearColor(0.6f, 0.45f, 0.6f, 1.0f);

    unsigned int shader = make_shader(
        "../src/shaders/vertex.txt",
        "../src/shaders/fragment.txt"
    );
    
    while (!glfwWindowShouldClose(window)) { // возвращает !false - пока окно не закрывается
        // Обрабатывает события пользователя, должна вызываться в каждом кадре
        glfwPollEvents();
        
        // Очистка буфера для получения черного цвета фона
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        
        // Обмен буферов
        // Сначала рисует в задний буфер, затем показывает его пользователю при этом меняясь местами
        glfwSwapBuffers(window);
    }
    
    glDeleteProgram(shader);
    // Освобождает ресурсы glfw
    glfwTerminate();
    return 0;
}



