#include "renderer.h"
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "defshdr.h"

/* --------------------------------------------------
 * Renders a screen space quad
 * -------------------------------------------------- */
static void render_quad()
{
    GLfloat quad_vert[] =
    {
       -1.0f,  1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

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
    glShaderSource(vert_shader, 1, &def_vert_sh_src, 0);
    glCompileShader(vert_shader);
    check_last_compile_error(vert_shader);
    ctx->vert_shader = vert_shader;

    /* Compile fragment shader */
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &def_frag_sh_src, 0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Setup uniforms */
    glUseProgram(ctx->program);
    glBindAttribLocation(ctx->program, 0, "position");

    long long timer_value = get_timer_value();
    float time = ((timer_value / 30000) % 200) / 100.0f;
    glUniform1f(glGetUniformLocation(ctx->program, "time"), time);
    glUniform2f(glGetUniformLocation(ctx->program, "resolution"), 800.0f, 600.0f);

    /* Render */
    render_quad();
    glUseProgram(0);
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
