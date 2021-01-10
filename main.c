#define GL_GLEXT_PROTOTYPES
#define ERROR_OPEN_FILE -3

#include <SDL.h>
#include <SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <time.h>
#include <math.h>

#include <stdio.h>

typedef float t_mat4x4[16];

static inline void mat4x4_ortho(t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar){
	#define T(a, b) (a * 4 + b)

	out[T(0,0)] = 2.0f / (right - left);
	out[T(0,1)] = 0.0f;
	out[T(0,2)] = 0.0f;
	out[T(0,3)] = 0.0f;

	out[T(1,1)] = 2.0f / (top - bottom);
	out[T(1,0)] = 0.0f;
	out[T(1,2)] = 0.0f;
	out[T(1,3)] = 0.0f;

	out[T(2,2)] = -2.0f / (zfar - znear);
	out[T(2,0)] = 0.0f;
	out[T(2,1)] = 0.0f;
	out[T(2,3)] = 0.0f;

	out[T(3,0)] = -(right + left) / (right - left);
	out[T(3,1)] = -(top + bottom) / (top - bottom);
	out[T(3,2)] = -(zfar + znear) / (zfar - znear);
	out[T(3,3)] = 1.0f;

	#undef T
}

void loadFile(char* buffer, int size, char* fileName) {
	FILE* file = fopen(fileName, "r");
	if(file == NULL) {
		printf("Error opening file %s\n", fileName);
		exit(ERROR_OPEN_FILE);
	}
	int i = 0;
	while(i < size - 1) {
		int c = fgetc(file);
		if(c == EOF) break;
		buffer[i] = (char) c;
		i++;
	}
	buffer[i] = '\0';
	fclose(file);
}


typedef enum t_attrib_id {
	attrib_position,
	attrib_color
} t_attrib_id;


typedef struct {
	float x;
	float y;
	float z;
} cameraDirection;

typedef struct {
	float x;
	float y;
	float z;
} cameraPosition;

void rotateCamera(cameraDirection* cd, float horizontal, float vertical){
	cd->y += vertical;
	float x = cd->x * cosf(horizontal) - cd->z * sinf(horizontal);
	float z = cd->x * sinf(horizontal) + cd->z * cosf(horizontal);
	cd->x = x;
	cd->z = z;
	x = sqrtf(cd->x * cd->x + cd->y * cd->y + cd->z * cd->z);
	cd->x /= x;
	cd->y /= x;
	cd->z /= x;
}

void moveForward(cameraDirection cd, cameraPosition* cp, float speed) {
	cp->x += cd.x * speed;
	cp->y += cd.y * speed;
	cp->z += cd.z * speed;
}

void moveBackward(cameraDirection cd, cameraPosition* cp, float speed) {
	cp->x -= cd.x * speed;
	cp->y -= cd.y * speed;
	cp->z -= cd.z * speed;
}

int main(int argc, char * argv[]) {
	int bufferSize = 2048;
	char *buffer = malloc(bufferSize * sizeof(char));

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	static const int width = 1920;
	static const int height = 1080;

	SDL_Window * window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLuint vs, fs, program;

	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	loadFile(buffer, bufferSize, "shaders/shader.vert");
	glShaderSource(vs, 1, (const GLchar **)&buffer, NULL);
	glCompileShader(vs);

	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		fprintf(stderr, "vertex shader compilation failed\n");
		return 1;
	}

	loadFile(buffer, bufferSize, "shaders/shader.frag");
	glShaderSource(fs, 1, (const GLchar **)&buffer, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE) {
		fprintf(stderr, "fragment shader compilation failed\n");
		return 1;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glBindAttribLocation(program, attrib_position, "i_position");
	glBindAttribLocation(program, attrib_color, "i_color");
	glLinkProgram(program);

	glUseProgram(program);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.0, 0.0, 0.0);
	glViewport(0, 0, width, height);

	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	glEnableVertexAttribArray(attrib_position);
	glEnableVertexAttribArray(attrib_color);

	glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(4 * sizeof(float)));

	const GLfloat g_vertex_buffer_data[] = {
	/*  R, G, B, A, X, Y  */
		1, 0, 0, 1, 0, 0,
		0, 1, 0, 1, width, 0,
		0, 0, 1, 1, width, height,

		1, 0, 0, 1, 0, 0,
		0, 0, 1, 1, width, height,
		1, 1, 1, 1, 0, height
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	
	GLuint colors;
	glGenTextures(1, &colors);
	glBindTexture(GL_TEXTURE_1D, colors);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int vW, vH;
	unsigned char* image = SOIL_load_image("images/colors.png", &vW, &vH, NULL, SOIL_LOAD_RGBA);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, vW * vH, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_1D, 0);
	SOIL_free_image_data(image);

	GLuint voxels;
	struct voxel{
		GLuint index;
		GLuint color;
	};
	struct voxel voxelsBuffer[2];
	voxelsBuffer[0].color = 2;
	glGenBuffers(1, &voxels);
	glBindBuffer(GL_UNIFORM_BUFFER, voxels);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(struct voxel), 0, GL_STATIC_READ);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(struct voxel), &voxelsBuffer);

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "octree"), 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, voxels); 
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_1D, colors);
	glUniform1i(glGetUniformLocation(program, "colors"), 0);


	t_mat4x4 projection_matrix;
	mat4x4_ortho(projection_matrix, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix"), 1, GL_FALSE, projection_matrix);
	glUniform2f(glGetUniformLocation(program, "iResolution"), (GLfloat)width, (GLfloat)height);
	glUniform1f(glGetUniformLocation(program, "iTime"), (GLfloat)clock());
	glUniform2f(glGetUniformLocation(program, "iMouse"), (GLfloat)0, (GLfloat)0);

	cameraDirection cd = {0};
	cameraPosition cp = {-2, -2, 0};
	cd.x = 1;

	glUniform3f(glGetUniformLocation(program, "iCameraDirection"),
		(GLfloat)cd.x,
		(GLfloat)cd.y,
		(GLfloat)cd.z);
	glUniform3f(glGetUniformLocation(program, "iCameraPosition"),
		(GLfloat)cp.x,
		(GLfloat)cp.y,
		(GLfloat)cp.z);

	SDL_SetRelativeMouseMode(SDL_TRUE);
	for(;;) {
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE)
						return 0;
					else if(event.key.keysym.sym == SDLK_w){
						moveForward(cd, &cp, 0.5);
					}
					else if(event.key.keysym.sym == SDLK_s){
						moveBackward(cd, &cp, 0.5);
					}
					break;
				case SDL_MOUSEMOTION:
					glUniform2f(glGetUniformLocation(program, "iMouse"), (GLfloat)event.motion.x, (GLfloat)event.motion.y);
					glUniform3f(glGetUniformLocation(program, "iCameraDirection"),
						(GLfloat)cd.x,
						(GLfloat)cd.y,
						(GLfloat)cd.z);
					rotateCamera(&cd, -(float)event.motion.xrel / 800.0, -(float)event.motion.yrel / 800.0);
					break;

			}
		}

		glUniform1f(glGetUniformLocation(program, "iTime"), (GLfloat)clock());
		glUniform3f(glGetUniformLocation(program, "iCameraPosition"),
			(GLfloat)cp.x,
			(GLfloat)cp.y,
			(GLfloat)cp.z);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(window);
		SDL_Delay(1);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

