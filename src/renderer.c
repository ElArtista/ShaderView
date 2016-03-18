#include "renderer.h"
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

const GLchar* vert_shader_src =
"#version 330 core                        \n\
in vec3 position;                         \n\
in vec2 uvCoords;                         \n\
                                          \n\
out vec2 UVCoords;                        \n\
                                          \n\
void main(void)                           \n\
{                                         \n\
    UVCoords = uvCoords;                  \n\
    gl_Position = vec4(position, 1.0f);   \n\
}";

const GLchar* frag_shader_src =
"#version 330 core                                 \n\
out vec4 color;                                    \n\
in vec2 UVCoords;                                  \n\
                                                   \n\
uniform sampler2D imageData;                       \n\
                                                   \n\
void main(void)                                    \n\
{                                                  \n\
    vec4 idata = texture(imageData, UVCoords);     \n\
    color = idata;                                 \n\
}";

/* --------------------------------------------------
 * Renders a screen space quad
 * -------------------------------------------------- */
static void render_quad()
{
    GLfloat quad_vert[] =
    {
       -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
       -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    /* Setup vertex data */
    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert), &quad_vert, GL_STATIC_DRAW);

    /* Setup vertex attributes */
    GLuint quad_vao;
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /* Dispatch draw operation */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    /* Free uploaded resources */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteVertexArrays(1, &quad_vao);
}

/* --------------------------------------------------
 * Checks and shows last OpenGL error occurred
 * -------------------------------------------------- */
static void check_error()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        const GLubyte* err_desc = gluErrorString(err);
        fprintf(stderr, "OpenGL error: %s", err_desc);
    }
}

/* --------------------------------------------------
 * Checks and shows last shader compile error occurred
 * -------------------------------------------------- */
static void check_last_compile_error(GLuint id)
{
    /* Check if last compile was successful */
    GLint compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        /* Gather the compile log size */
        GLint logLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            /* Fetch and print log */
            GLchar* buf = malloc(logLength * sizeof(GLchar));
            glGetShaderInfoLog(id, logLength, 0, buf);
            fprintf(stderr, "Shader error: %s", buf);
            free(buf);
        }
    }
}

/* --------------------------------------------------
 * Checks and shows last shader link error occurred
 * -------------------------------------------------- */
static void check_last_link_error(GLuint id)
{
    /* Check if last link was successful */
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        /* Gather the link log size */
        GLint logLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            /* Fetch and print log */
            GLchar* buf = malloc(logLength * sizeof(GLchar));
            glGetProgramInfoLog(id, logLength, 0, buf);
            fprintf(stderr, "Shader program error: %s", buf);
            free(buf);
        }
    }
}

/* --------------------------------------------------
 * Uploads given texture data to the GPU
 * -------------------------------------------------- */
static GLuint load_texture(int width, int height, GLvoid* data)
{
    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    check_error();
    return id;
}

/* --------------------------------------------------
 * Initializes renderer state
 * -------------------------------------------------- */
void init_renderer(struct render_context* ctx)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, 800, 600);

    /* Compile vertex shader */
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_shader_src, 0);
    glCompileShader(vert_shader);
    check_last_compile_error(vert_shader);
    ctx->vert_shader = vert_shader;

    /* Compile fragment shader */
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_shader_src, 0);
    glCompileShader(frag_shader);
    check_last_compile_error(frag_shader);
    ctx->frag_shader = frag_shader;

    /* Link shader program */
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    check_last_link_error(shader_program);
    ctx->program = shader_program;
}

struct pixel
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
};

/* --------------------------------------------------
 * Main render function
 * -------------------------------------------------- */
void render(struct render_context* ctx)
{
    glClear(GL_COLOR_BUFFER_BIT);

    /* Create sample image data */
    struct pixel image_data[4];
    memset((void*)image_data, 0, sizeof(image_data));
    image_data[0].r = 100;
    image_data[0].b = 50;
    image_data[3].b = 255;

    /* Setup uniforms */
    glUseProgram(ctx->program);
    glBindAttribLocation(ctx->program, 0, "position");
    glBindAttribLocation(ctx->program, 1, "uvCoords");
    GLuint img_loc = glGetUniformLocation(ctx->program, "imageData");
    glUniform1i(img_loc, 0);

    /* Bind texture */
    GLuint texture = load_texture(2, 2, (GLvoid*) image_data);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    /* Render */
    render_quad();
    glUseProgram(0);

    /* Free sample texture */
    glDeleteTextures(1, &texture);
}

/* --------------------------------------------------
 * Frees renderer resources
 * -------------------------------------------------- */
void destroy_renderer(struct render_context* ctx)
{
    glDeleteProgram(ctx->program);
    glDeleteShader(ctx->vert_shader);
    glDeleteShader(ctx->frag_shader);
}
