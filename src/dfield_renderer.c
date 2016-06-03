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

#define TEXTURE_FILE "ear.dfield"

static const float vertex_data[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f
};

static const char *vertex_shader_source =
"#version 330\n"
"uniform vec2 scale;\n"
"in vec3 position;\n"
"out vec2 texture_coord;\n"
"void main () {\n"
"	vec2 outp = position.xy * scale;\n"
"	gl_Position = vec4(outp, position.z, 1.0);\n"
"	texture_coord = ((position.xy / scale) + vec2(1.0)) / vec2(2.0);\n"
"	texture_coord.y = 1.0 - texture_coord.y;\n"
"}";

static const char *fragment_shader_source =
"#version 330\n"
"uniform sampler2D texture_unit;\n"
"out vec4 frag_color;\n"
"in vec2 texture_coord;\n"
"void main () {\n"
"	float color = texture(texture_unit, texture_coord).r;\n"
"	if(color > 0.1){\n"
"		frag_color = vec4(1.0);\n"
"	}else{\n"
"		frag_color = vec4(0.0, 0.0, 0.0, 1.0);\n"
"	}\n"
"}";

char texture_width, texture_height;
char *texture_raw;

void read_texture(void)
{
	FILE *file;
	size_t bytes;

	/* Open texture */
	file = fopen(TEXTURE_FILE, "rb");
	if(!file){
		cc_set_error("Could not open %s", TEXTURE_FILE);
	}
	fseek(file, 0, SEEK_END);
  bytes = ftell(file);
  rewind(file);

	fread(&texture_width, sizeof(char), 1, file);
	fread(&texture_height, sizeof(char), 1, file);

	if(bytes != (unsigned long)(texture_width * texture_height) + 2){
		cc_set_error("File size not correct");
	}

	texture_raw = (char*)malloc(bytes - 2);
	if(!texture_raw){
		cc_out_of_memory_error();
	}

	bytes = fread(texture_raw, sizeof(char), bytes - 2, file);
	if(bytes != (unsigned long)(texture_width * texture_height)){
		cc_set_error("Error reading bytes from file, could only read %lu bytes", bytes);
	}
	fclose(file);
}

void compile_and_check_shader(GLuint shader, const char *shader_source)
{
	int shader_status, shader_log_length;
	char *shader_log;

	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);
  if(!shader_status) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shader_log_length);
		shader_log = (char*)malloc(shader_log_length + 1);
		glGetShaderInfoLog(shader, shader_log_length, NULL, shader_log);

		fprintf(stderr, "Compilation error in OpenGL shader:\n%s\nSource:\n%s\n", shader_log, shader_source);

		exit(EXIT_FAILURE);
	}
}

void error_handler(const char *message)
{
	fprintf(stderr, "Error: \"%s\"\n", cc_get_error());
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	GLint scale_location, texture_location;
	GLuint texture, vertex_buffer_object, vertex_array_object, vertex_shader, fragment_shader, program;
	struct cc_event event;

	cc_set_error_handler(error_handler);

	read_texture();

	cc_new_window(0);

	cc_set_window_size(800, 600);
	cc_set_window_title("Distance field renderer");

	cc_bind_opengl_context();

#ifdef USE_GLEW
	glewInit();
#endif	
	
	/* Setup OpenGL */
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertex_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray (vertex_array_object);
	glEnableVertexAttribArray (0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	vertex_shader = glCreateShader (GL_VERTEX_SHADER);
	compile_and_check_shader(vertex_shader, vertex_shader_source);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	compile_and_check_shader(fragment_shader, fragment_shader_source);

	program = glCreateProgram();
	glAttachShader(program, fragment_shader);
	glAttachShader(program, vertex_shader);
	glLinkProgram(program);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, texture_raw);
	free(texture_raw);
	glBindTexture(GL_TEXTURE_2D, 0);

	scale_location = glGetUniformLocation(program, "scale");
	if(scale_location < 0){
		cc_set_error("Could not get OpenGL uniform location");
	}

	texture_location = glGetUniformLocation(program, "texture_unit");
	if(texture_location < 0){
		cc_set_error("Could not get OpenGL uniform location");
	}

	while(cc_poll_window()){
		event = cc_pop_event();
		if(event.type == CC_EVENT_DRAW){
			glViewport(0, 0, cc_get_window_width(), cc_get_window_height());

			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);

			glUseProgram(program);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(texture_location, 0);

			glUniform2f(scale_location, 1.0, 1.0);
			glBindVertexArray(vertex_array_object);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			cc_swap_opengl_buffers();
		}
	}

	cc_destroy_opengl_context();
	cc_destroy_window();

	return EXIT_SUCCESS;
}
