#include <myRenderInterface.h>

#define GL_CLAMP_TO_EDGE 0x812F

myRenderInterface::myRenderInterface(unsigned Width, unsigned Height, std::string* windowName) : RenderInterface{} {
	//OpenGL init code
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	screen = glfwCreateWindow(Width, Height, windowName->c_str(), NULL, NULL);

	glfwMakeContextCurrent(screen);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	Projection = glm::ortho(0.0f, (float)Width, (float)Height, 0.0f, -10000.0f, 10000.0f);


	Model = glm::mat4(1.0f);
	View = glm::mat4(1.0f);

	MVP = Projection * View * Model;

	glViewport(0, 0, Width, Height);
	glfwSetMouseButtonCallback(screen, mouse_button_callback);
	glfwSetCursorPosCallback(screen, cursor_position_callback);
	
	//drawing setup part for textures + color
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);

	shader = new Shader("3.3.shader.vs", "3.3.shader.fs");

	//...and color only
	glGenVertexArrays(1, &CVAO);
	glGenBuffers(1, &CEBO);
	glGenBuffers(1, &CVBO);

	glBindVertexArray(CVAO);
	glBindBuffer(GL_ARRAY_BUFFER, CVBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	col_shader = new Shader("3.3.shader_col.vs", "3.3.shader_col.fs");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void myRenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{

	unsigned int* actualIndicies = new unsigned int[num_indices];
	unsigned int idxIncr = -4;

	//std::cout << "Indicies: "<< std::endl;

	for (int i = 0; i < num_indices; i++) {
		if (i % 6 == 0)
			idxIncr += 4;

		actualIndicies[i] = ref_indices[i % 6] + idxIncr;
		//std::cout << actualIndicies[i] << " vs " << indices[i] << std::endl;
	}


	glm::mat4 translationMat = glm::translate(glm::mat4(1), glm::vec3(translation.x, translation.y, 0));

	glm::vec4 transformedVector;

	glm::vec4 originalVector;
	
	RenderVector2f* translatedVector = new RenderVector2f[num_vertices];
	ColRenderVector2f* translatedVectorCol = new ColRenderVector2f[num_vertices];

	float r, g, b, a;

	for (int i = 0; i < num_vertices; i++) {

		originalVector.x = vertices[i].position.x;
		originalVector.y = vertices[i].position.y;
		originalVector.z = 0;
		originalVector.w = 1;

		transformedVector = originalVector + glm::vec4(translation.x, translation.y, 0, 0);
		transformedVector = MVP * transformedVector;
		
		//std::cout << "vertex " << i << ": (" << transformedVector.x << ", " << transformedVector.y << ")" << std::endl;

		r = vertices[i].colour.red; g = vertices[i].colour.green;
		b = vertices[i].colour.blue; a = vertices[i].colour.alpha;
		r = (r / 127.5) - 1;
		g = (g / 127.5) - 1;
		b = (b / 127.5) - 1;
		a = (a / 127.5) - 1;

		if (texture) {
			translatedVector[i] = {
				transformedVector.x, transformedVector.y,
				vertices[i].tex_coord.x, vertices[i].tex_coord.y,
				r, g, b, a
			};
		} else {
 			translatedVectorCol[i] = {
				transformedVector.x, transformedVector.y,
				r, g, b, a
			};
		}
	}

	//std::cout << std::endl;

	if (texture) {
		shader->use();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBindTexture(GL_TEXTURE_2D, (GLuint)texture);

		glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVector2f) * num_vertices, translatedVector, GL_DYNAMIC_DRAW);

	} else {
		col_shader->use();
		glBindVertexArray(CVAO);
		glBindBuffer(GL_ARRAY_BUFFER, CVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(ColRenderVector2f) * num_vertices, translatedVectorCol, GL_DYNAMIC_DRAW);
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

};

Rml::CompiledGeometryHandle myRenderInterface::CompileGeometry(Rml::Vertex * vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) {

	return (Rml::CompiledGeometryHandle) NULL;
};

void myRenderInterface::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f & translation) {
	std::cout << "Nothing is happening here" << std::endl;
};

void myRenderInterface::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) {

};

// Called by Rocket when it wants to enable or disable scissoring to clip content.		
void myRenderInterface::EnableScissorRegion(bool enable)
{
	if (enable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

// Called by Rocket when it wants to change the scissor region.		
void myRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
	glScissor(x, 0 - (y + height), width, height);
}

// Set to byte packing, or the compiler will expand our struct, which means it won't read correctly from file
#pragma pack(1) 
struct TGAHeader
{
	char  idLength;
	char  colourMapType;
	char  dataType;
	short int colourMapOrigin;
	short int colourMapLength;
	char  colourMapDepth;
	short int xOrigin;
	short int yOrigin;
	short int width;
	short int height;
	char  bitsPerPixel;
	char  imageDescriptor;
};
// Restore packing
#pragma pack()

bool myRenderInterface::LoadTexture(Rml::TextureHandle & texture_handle, Rml::Vector2i & texture_dimensions, const Rml::String & source)
{
	Rml::FileInterface* file_interface = Rml::GetFileInterface();
	Rml::FileHandle file_handle = file_interface->Open(source);
	if (!file_handle)
	{
		return false;
	}

	file_interface->Seek(file_handle, 0, SEEK_END);
	size_t buffer_size = file_interface->Tell(file_handle);
	file_interface->Seek(file_handle, 0, SEEK_SET);

	//ROCKET_ASSERTMSG(buffer_size > sizeof(TGAHeader), "Texture file size is smaller than TGAHeader, file must be corrupt or otherwise invalid");
	if (buffer_size <= sizeof(TGAHeader))
	{
		file_interface->Close(file_handle);
		return false;
	}

	char* buffer = new char[buffer_size];
	file_interface->Read(buffer, buffer_size, file_handle);
	file_interface->Close(file_handle);

	TGAHeader header;
	memcpy(&header, buffer, sizeof(TGAHeader));

	int color_mode = header.bitsPerPixel / 8;
	int image_size = header.width * header.height * 4; // We always make 32bit textures 

	if (header.dataType != 2)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
		return false;
	}

	// Ensure we have at least 3 colors
	if (color_mode < 3)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24 and 32bit textures are supported");
		return false;
	}

	const char* image_src = buffer + sizeof(TGAHeader);
	unsigned char* image_dest = new unsigned char[image_size];

	// Targa is BGR, swap to RGB and flip Y axis
	for (long y = 0; y < header.height; y++)
	{
		long read_index = y * header.width * color_mode;
		long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
		for (long x = 0; x < header.width; x++)
		{
			image_dest[write_index] = image_src[read_index + 2];
			image_dest[write_index + 1] = image_src[read_index + 1];
			image_dest[write_index + 2] = image_src[read_index];
			if (color_mode == 4)
				image_dest[write_index + 3] = image_src[read_index + 3];
			else
				image_dest[write_index + 3] = 255;

			write_index += 4;
			read_index += color_mode;
		}
	}

	texture_dimensions.x = header.width;
	texture_dimensions.y = header.height;

	bool success = GenerateTexture(texture_handle, image_dest, texture_dimensions);

	delete[] image_dest;
	delete[] buffer;

	return success;
};

bool  myRenderInterface::GenerateTexture(Rml::TextureHandle & texture_handle, const Rml::byte * source, const Rml::Vector2i & source_dimensions)
{
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	if (texture_id == 0)
	{
		printf("Failed to generate textures\n");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	texture_handle = (Rml::TextureHandle) texture_id;

	return true;
};

void myRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle)
{
	glDeleteTextures(1, (GLuint*)& texture_handle);
}

void myRenderInterface::LoadFonts(const char* directory) {

	Rml::String font_names[5];
	font_names[0] = "Delicious-Roman.otf";
	font_names[1] = "Delicious-Italic.otf";
	font_names[2] = "Delicious-Bold.otf";
	font_names[3] = "Delicious-BoldItalic.otf";
	font_names[4] = "NotoEmoji-Regular.ttf";

	const int fallback_face = 4;

	for (size_t i = 0; i < sizeof(font_names) / sizeof(Rml::String); i++)
	{
		Rml::LoadFontFace(Rml::String(directory) + font_names[i], i == fallback_face);
	}
}

Rml::Context* myRenderInterface::context = NULL;