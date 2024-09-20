/**************************************************************************
 * @file Shader.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows easy usage of Shader System for Graphics Shaders.
 *
 *
 *************************************************************************/

#include "Shader.h"


 /**************************************************************************
 * @brief		Link the Shader
 *
 * @param none
 * @return GLboolean
 *************************************************************************/
GLboolean OpenGLShader::Link() {
    if (GL_TRUE == is_linked) {
        //std::cout << "link fail\n";
        return GL_TRUE;
    }
    if (pgm_handle <= 0) {
        //std::cout << "pgm handle not >0\n";
        return GL_FALSE;
    }

    glLinkProgram(pgm_handle); // link the various compiled shaders

    // verify the link status
    GLint lnk_status;
    glGetProgramiv(pgm_handle, GL_LINK_STATUS, &lnk_status);
    if (GL_FALSE == lnk_status) {
        log_string = "Failed to link shader program\n";
        GLint log_len;
        glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_str = new GLchar[log_len];
            GLsizei written_log_len;
            glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
            log_string += log_str;
            delete[] log_str;
        }
        return GL_FALSE;
    }
    return is_linked = GL_TRUE;
}
/**************************************************************************
* @brief		Get Handle of the Shader
*
* @param none
* @return GLuint
*************************************************************************/
GLuint OpenGLShader::GetHandle() const {
    return pgm_handle;
}
/**************************************************************************
* @brief		Validate the Shader
*
* @param NONE
* @return GLboolean
*************************************************************************/
GLboolean OpenGLShader::Validate() {
    if (pgm_handle <= 0 || is_linked == GL_FALSE) {
        return GL_FALSE;
    }

    glValidateProgram(pgm_handle);
    GLint status;
    glGetProgramiv(pgm_handle, GL_VALIDATE_STATUS, &status);
    if (GL_FALSE == status) {
        log_string = "Failed to validate shader program for current OpenGL context\n";
        GLint log_len;
        glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_str = new GLchar[log_len];
            GLsizei written_log_len;
            glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
            log_string += log_str;
            delete[] log_str;
        }
        return GL_FALSE;
    }
    else {
        return GL_TRUE;
    }
}
/**************************************************************************
* @brief		Gets Log of the shader if there are any errors
*
* @param NONE
* @return std::string
*************************************************************************/
std::string OpenGLShader::GetLog() const {
    return log_string;
}
/**************************************************************************
* @brief		Check if the Shader is linked.
*
* @param NONE
* @return GLboolean
*************************************************************************/
GLboolean OpenGLShader::IsLinked() const {
    return is_linked;
}
/**************************************************************************
* @brief		CompileLinkValidate
*
* @param std::vector<std::pair<GLenum, std::string>>
* @return void
*************************************************************************/
GLboolean OpenGLShader::CompileLinkValidate(std::vector<std::pair<GLenum, std::string>> vec) {
#ifdef _DEBUG
    

    for (auto& elem : vec) {
        std::cout << "Element First and Second elements: " << elem.first << '\t';
        std::cout << elem.second.c_str() << '\n';

        if (GL_FALSE == CompileShaderFromFile(elem.first, elem.second.c_str())) {
            std::cout << "Failed to Compile Shader from File\n";
            return GL_FALSE;
        }
    }
    if (GL_FALSE == Link()) {
        std::cout << "Linking\n";
        return GL_FALSE;
    }
    if (GL_FALSE == Validate()) {
        std::cout << "Validate\n";
        return GL_FALSE;
    }
    std::cout << "Shader has been loaded\n";
    std::cout << "\nShader Parameters are as shown:\n";
    PrintActiveAttribs();
    PrintActiveUniforms();
    return GL_TRUE;

#endif
}

/**************************************************************************
* @brief		Compile Shader From File
*
* @param GLenum 	shader Type i.e. Frag Shader | Vert Shader
* @param std::string string of the file name
*
* @return void
*************************************************************************/
GLboolean OpenGLShader::CompileShaderFromFile(GLenum shader_type, const std::string& file_name) {
   
    if (GL_FALSE == FileExists(file_name)) {
        //std::cout << "not found\n";
        log_string = "File not found";
        return GL_FALSE;
    }

    if (pgm_handle <= 0) {
        std::cout << "Create Program\n";

        pgm_handle = glCreateProgram();
        if (0 == pgm_handle) {
            std::cout << "program handle failed\n";
            log_string = "Cannot create program handle";
            return GL_FALSE;
        }
    }

    std::ifstream shader_file(file_name, std::ifstream::in);
    if (!shader_file) {
        //std::cout << "Error Open File\n";
        log_string = "Error opening file " + file_name;
        return GL_FALSE;
    }
    std::stringstream buffer;
    buffer << shader_file.rdbuf();
    shader_file.close();

   // std::cout << "it reached here\n";
    return CompileShaderFromString(shader_type, buffer.str());
}
/**************************************************************************
* @brief		Deletes Shader Program
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void
OpenGLShader::DeleteShaderProgram() {
    if (pgm_handle > 0) {
        glDeleteProgram(pgm_handle);
    }
}

/**************************************************************************
* @brief		Compile Shader from String
*
* @param GLenum		shader Type i.e. Frag Shader | Vert Shader
* @param std::string string of the shader soruce
*
* @return GLboolean
*************************************************************************/
GLboolean
OpenGLShader::CompileShaderFromString(GLenum shader_type,
    const std::string& shader_src) {


    if (pgm_handle <= 0) {
    
     //   std::cout << "Create Program\n";
        pgm_handle = glCreateProgram();
        if (0 == pgm_handle) {
            log_string = "Cannot create program handle";
            return GL_FALSE;
        }
    }

    GLuint shader_handle = 0;
    switch (shader_type) {
    case VERTEX_SHADER: shader_handle = glCreateShader(GL_VERTEX_SHADER); break;
    case FRAGMENT_SHADER: shader_handle = glCreateShader(GL_FRAGMENT_SHADER); break;
    case GEOMETRY_SHADER: shader_handle = glCreateShader(GL_GEOMETRY_SHADER); break;
    case TESS_CONTROL_SHADER: shader_handle = glCreateShader(GL_TESS_CONTROL_SHADER); break;
    case TESS_EVALUATION_SHADER: shader_handle = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
        //case COMPUTE_SHADER: shader_handle = glCreateShader(GL_COMPUTE_SHADER); break;
    default:
        log_string = "Incorrect shader type";
        return GL_FALSE;
    }

    // load shader source code into shader object
    GLchar const* shader_code[] = { shader_src.c_str() };
    glShaderSource(shader_handle, 1, shader_code, NULL);
    //std::cout << "compiling\n";
    // compile the shader
    glCompileShader(shader_handle);
   // std::cout << "compiling finish\n";
    // check compilation status
    GLint comp_result;

//    std::cout << shader_handle << '\n';
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &comp_result);


    if (GL_FALSE == comp_result) {
    
        //std::cout << "Vertex Shader Comile Fail\n";
        log_string = "Vertex shader compilation failed\n";
        GLint log_len;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log = new GLchar[log_len];
            GLsizei written_log_len;
            glGetShaderInfoLog(shader_handle, log_len, &written_log_len, log);
            log_string += log;
            delete[] log;
        }

        std::cout << log_string <<'\n';
        return GL_FALSE;
    }
    else { // attach the shader to the program object
        glAttachShader(pgm_handle, shader_handle);
        return GL_TRUE;
    }
}
/**************************************************************************
* @brief		Use the Shader
*
* @param none
* @return void
*************************************************************************/
void OpenGLShader::Use() {
    if (pgm_handle > 0 && is_linked == GL_TRUE) {
        glUseProgram(pgm_handle);
    }
}
/**************************************************************************
    * @brief		Unuse the shader
    *
    * @param none
    * @return void
    *************************************************************************/
void OpenGLShader::UnUse() {
    glUseProgram(0);
}

/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, GLboolean val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, GLint val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, GLfloat val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform1f(loc, val);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform2f(loc, x, y);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    //std::cout << loc << '\n';

    if (loc >= 0) {
        //std::cout << loc << '\n';
        glUniform3f(loc, x, y, z);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void
OpenGLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform4f(loc, x, y, z, w);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, glm::vec2 const& val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform2f(loc, val.x, val.y);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, glm::vec3 const& val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform3f(loc, val.x, val.y, val.z);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, glm::vec4 const& val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniform4f(loc, val.x, val.y, val.z, val.w);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, glm::mat3 const& val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		Gets Uniform Location
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::SetUniform(GLchar const* name, glm::mat4 const& val) {
    GLint loc = glGetUniformLocation(pgm_handle, name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
    }
    else {
        std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
}
/**************************************************************************
* @brief		display the list of active vertex attributes used by vertex
                shader
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::PrintActiveAttribs() const {
#if 1
    GLint max_length, num_attribs;
    glGetProgramiv(pgm_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
    glGetProgramiv(pgm_handle, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    GLchar* pname = new GLchar[max_length];
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Index\t\t|\tName\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_attribs; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveAttrib(pgm_handle, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetAttribLocation(pgm_handle, pname);
        std::cout << loc << "\t\t\t" << pname << std::endl;
    }
    std::cout << "----------------------------------------------------------------------\n";
    delete[] pname;
#else
    GLint numAttribs;
    glGetProgramInterfaceiv(pgm_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    std::cout << "Active attributes:" << std::endl;
    for (GLint i = 0; i < numAttribs; ++i) {
        GLint results[3];
        glGetProgramResourceiv(pgm_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

        GLint nameBufSize = results[0] + 1;
        GLchar* pname = new GLchar[nameBufSize];
        glGetProgramResourceName(pgm_handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, pname);
        //   std::cout << results[2] << " " << pname << " " << getTypeString(results[1]) << std::endl;
        std::cout << results[2] << " " << pname << " " << results[1] << std::endl;
        delete[] pname;
    }
#endif
}
/**************************************************************************
* @brief		display the list of active uniform variables
*
* @param name
* @param ...   boolean | int | float | x,y, | x,y,z, | x,y,z,w | vec2 |
*				vec3| vec4 | mat3 | mat4
* @return void
*************************************************************************/
void OpenGLShader::PrintActiveUniforms() const {
    GLint max_length;
    glGetProgramiv(pgm_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    GLchar* pname = new GLchar[max_length];
    GLint num_uniforms;
    glGetProgramiv(pgm_handle, GL_ACTIVE_UNIFORMS, &num_uniforms);
    std::cout << "Location\t|\tName\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_uniforms; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveUniform(pgm_handle, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetUniformLocation(pgm_handle, pname);
        std::cout << loc << "\t\t\t" << pname << std::endl;
    }
    std::cout << "----------------------------------------------------------------------\n";
    delete[] pname;
}


GLint OpenGLShader::GetUniformLocation(GLchar const* name) {
    return glGetUniformLocation(pgm_handle, name);
}

GLboolean OpenGLShader::FileExists(std::string const& file_name) {
    std::ifstream infile(file_name); return infile.good();
}
