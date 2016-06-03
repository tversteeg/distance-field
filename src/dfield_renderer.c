#include <ccore.h>
#include <cc_window.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_EPOXY
#include <epoxy/gl.h>
#elif defined USE_GLEW
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

static const float vertex_data[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f
};

static const char *vertex_shader_source =
"#version 330\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

static const char *fragment_shader_source =
"#version 330\n"
"out vec4 frag_colour;"
"void main () {"
"  frag_colour = vec4 (1.0, 1.0, 1.0, 1.0);"
"}";

void error_handler(const char *message)
{
	fprintf(stderr, "Error: \"%s\"\n", cc_get_error());
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	GLuint vertex_buffer_object, vertex_array_object, vertex_shader, fragment_shader, program;
	struct cc_event event;

	cc_set_error_handler(error_handler);

	cc_new_window(0);

	cc_set_window_size(800, 600);
	cc_set_window_title("Distance field renderer");

	cc_bind_opengl_context();

#ifdef USE_GLEW
	glewInit();
#endif	
	
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertex_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray (vertex_array_object);
	glEnableVertexAttribArray (0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	vertex_shader = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, fragment_shader);
	glAttachShader(program, vertex_shader);
	glLinkProgram(program);

	while(cc_poll_window()){
		event = cc_pop_event();
		if(event.type == CC_EVENT_DRAW){
			glViewport(0, 0, cc_get_window_width(), cc_get_window_height());

			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);

			glUseProgram(program);
			glBindVertexArray(vertex_array_object);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			cc_swap_opengl_buffers();
		}
	}

	cc_destroy_opengl_context();
	cc_destroy_window();

	return EXIT_SUCCESS;
}
