
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>


struct TexRenderVector2f {
	float x, y, TexX, TexY;
};

struct RenderVector2f {
	float x, y, TexX, TexY, red, green, blue, alpha;
};

//struct ColRenderVector2f {
//	float x, y;
//	unsigned char red, green, blue, alpha;
//};

struct ColRenderVector2f {
	float x, y, red, green, blue, alpha;
};

static unsigned int ref_indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3,    // second triangle
	4, 5, 7,   // first triangle
	5, 6, 7,   // second triangle

	8, 9, 11,   // first triangle
	9, 10, 11,    // second triangle
	12, 13, 15,   // first triangle
	13, 14, 15   // second triangle
};

class myRenderInterface : public Rml::RenderInterface {

	GLFWwindow* screen;

	unsigned int VBO, VAO, EBO, CEBO, CVBO, CVAO;

	Shader * shader, * col_shader;

	glm::mat4 Projection;

	glm::mat4 View;
	glm::mat4 Model;

	glm::mat4 MVP;

public:

	static Rml::Context* context;

	myRenderInterface(unsigned Width, unsigned Height, std::string* windowName);

	GLFWwindow* getScreen() { return screen; };

	// Called by libRocket when it wants to render geometry that the application does not wish to optimise.
	virtual void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation);


	// Called by libRocket when it wants to compile geometry it believes will be static for the forseeable future.
	virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture);

	// Called by libRocket when it wants to render application-compiled geometry.
	virtual void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation);

	// Called by libRocket when it wants to release application-compiled geometry.
	virtual void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry);

	// Called by libRocket when it wants to enable or disable scissoring to clip content.
	virtual void EnableScissorRegion(bool enable);

	// Called by libRocket when it wants to change the scissor region.
	virtual void SetScissorRegion(int x, int y, int width, int height);

	// Called by libRocket when a texture is required by the library.
	virtual bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source);

	// Called by libRocket when a texture is required to be built from an internally-generated sequence of pixels.
	virtual bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions);

	// Called by libRocket when a loaded texture is no longer required.
	virtual void ReleaseTexture(Rml::TextureHandle texture_handle);

	/// Loads the default fonts from the given path.
	void LoadFonts(const char* directory);


	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS) {
			context->ProcessMouseButtonDown(button, 0);
		}
		if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE) {
			context->ProcessMouseButtonUp(button, 0);
		}
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		context->ProcessMouseMove(xpos, ypos, 0);
	}

};