#include "config.h"
#include "glad/glad.h"
#include "material.h"
#include "triangle_mesh.h"
#include "linear_algs.h"

unsigned int make_module(const std::string &filepath,
                         unsigned int module_type) {
  std::ifstream file;              // поток для чтения из файла
  std::stringstream bufferedLines; // буфер строк
  std::string line;                // строка для чтения

  file.open(filepath); // открываем файл по пути
  while (std::getline(file, line)) {
    bufferedLines << line
                  << "\n"; // построчно читаем текст, аккумулируем строки в
                           // буфер, чтобы получить единый блок кода
  }
  std::string shaderSource =
      bufferedLines.str(); // возвращает строку со всем кодом
  const char *shaderSrc =
      shaderSource.c_str(); // преобразует строку в чары для Api openGL
  bufferedLines.str("");    // очищает строку
  file.close();             // закрывает файл

  unsigned int shaderModule = glCreateShader(
      module_type); // создает объект шейдера нужного типа. Возвращает GLuint
  glShaderSource(
      shaderModule, 1, &shaderSrc,
      NULL); // связывает ваш массив из одной C-строки (1) с этим объектом
  // реплейсит shaderModule код на новый
  glCompileShader(
      shaderModule); // компилирует GLSL-код в машинные инструкции GPU

  int success;
  glGetShaderiv(shaderModule, GL_COMPILE_STATUS,
                &success); // проверяем скомпилился ли шейдер
  if (!success) {
    char errorLog[1024];
    glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
    std::cout << "shader module compilation error:\n" << errorLog << std::endl;
  }

  return shaderModule; // возвращается валидный дескриптор шейдера, который
                       // дальше будут линковать в программу
}

unsigned int make_shader(const std::string &vertex_filepath,
                         const std::string &fragment_filepath) {
  std::vector<unsigned int> modules;
  modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
  modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

  // в modules складываем два шейдера
  // для каждого вызываем make_module
  unsigned int shader =
      glCreateProgram(); // создаем программу, в которой объединены оба шейдера
  for (unsigned int shaderModule : modules) {
    glAttachShader(shader,
                   shaderModule); // присоединяет каждый скомпилированный шейдер
                                  // к этой программе
  }
  glLinkProgram(shader); // связывает вместе входы/выходы обоих шейдеров и
                         // формирует единый испольняемый модуль на gpu

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
  if (!glfwInit()) {
    std::cout << "GLFW couldn't start" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Window", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    return -1;
  }

  glClearColor(0.6f, 0.45f, 0.6f, 1.0f);

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);

  TriangleMesh *triangle = new TriangleMesh();
  Material *material = new Material("../img/image.jpeg");
  Material *vignette = new Material("../img/vignette.jpeg");
  unsigned int shader =
      make_shader("../src/shaders/vertex.txt", "../src/shaders/fragment.txt");

  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "material"), 0);
  glUniform1i(glGetUniformLocation(shader, "vignette"), 1);

  vec3 quad_position = {0.5f, -0.3f, 0.0f};
  vec3 camera_pos = {-0.3f, 0.0f, 0.2f};
  vec3 camera_target = {0.0f, 0.0f, 0.0f};
  
  unsigned int model_location = glGetUniformLocation(shader, "model");
  unsigned int view_location = glGetUniformLocation(shader, "view");

  mat4 view = create_look_at(camera_pos, camera_target);
  glUniformMatrix4fv(view_location, 1, GL_FALSE, view.entries);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    mat4 model = create_model_transform(quad_position, 10 * glfwGetTime());
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.entries);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);

    material->use(0);
    vignette->use(1);
    triangle->draw();

    glfwSwapBuffers(window);
  }

  glDeleteProgram(shader);
  delete triangle;
  delete material;
  delete vignette;
  glfwTerminate();
  return 0;
}
