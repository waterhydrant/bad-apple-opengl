#include <glad/glad.h>

class Shader {
  public:
    unsigned int id;
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();

    void use();
};