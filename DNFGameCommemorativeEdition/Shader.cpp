#include "Shader.h"

Shader::Shader() {}

Shader::~Shader()
{
    ClearShader();
}

void
Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
    CompileShader(vertexCode, fragmentCode);
}

void
Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
    CreateFromString(ReadFile(vertexLocation).c_str(), ReadFile(fragmentLocation).c_str());
}

std::string
Shader::ReadFile(const char* fileLocation)
{
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File does not exist", fileLocation);
        return "";
    }

    std::string line {""};
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();

    return content;
}

GLuint
Shader::GetProjectionLocation()
{
    return uniformProjection_;
}

GLuint
Shader::GetModelLocation()
{
    return uniformModel_;
}

void
Shader::UseShader()
{
    glUseProgram(shaderID_);
}

void
Shader::ClearShader()
{
    if (shaderID_ != 0) {
        glDeleteProgram(shaderID_);
        shaderID_ = 0;
    }

    uniformModel_ = 0;
    uniformProjection_ = 0;
}

void
Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
    // Programe sits on graphic card

    // id
    shaderID_ = glCreateProgram();

    if (!shaderID_) {
        printf("Error creating shader programe!\n");
        return;
    }

    AddShader(shaderID_, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID_, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] {0};

    // Link
    glLinkProgram(shaderID_);
    glGetProgramiv(shaderID_, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shaderID_, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
    }

    // Validate
    glValidateProgram(shaderID_);
    glGetProgramiv(shaderID_, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shaderID_, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
    }

    // Get uniform var location
    uniformModel_ = glGetUniformLocation(shaderID_, "model");
    uniformProjection_ = glGetUniformLocation(shaderID_, "projection");
}

void
Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] {0};

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
    }

    glAttachShader(theProgram, theShader);
}
