#include "glob/graphics.h"

// no move plz
#include <glad/glad.h>
// no move plz

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "camera/camera.hpp"
#include "shader.h"

namespace glob {
namespace {

ShaderProgram test_shader;
GLuint quad_vbo, quad_vao;
Camera camera{glm::vec3(-3,0,0), glm::vec3(0), 90, 16.f/9.f, 0.1f, 100.f};

}  // namespace

void Init() {
  test_shader.add("testshader.frag");
  test_shader.add("testshader.vert");
  test_shader.compile();

  glGenVertexArrays(1, &quad_vao);
  glBindVertexArray(quad_vao);

  std::vector<glm::vec3> vertices{
      {-1, -1, -1},
      {3, -1, 1},
      {-1, 3, 1},
  };

  glGenBuffers(1, &quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  int stride = sizeof(glm::vec3);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
}

void Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 cam_transform = camera.GetViewPerspectiveMatrix();

  //cam_transform = glm::perspective(glm::radians(90.f), 16.f/9.f, 0.1f, 100.f) * glm::lookAt(glm::vec3(3), glm::vec3(0), glm::vec3(0, 1, 0));

  glBindVertexArray(quad_vao);
  test_shader.use();
  test_shader.uniform("cam_transform", cam_transform);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

}  // namespace glob