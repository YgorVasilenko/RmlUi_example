#pragma comment(lib, "RmlCore.lib")

#include <RmlUi/Core.h>
#include <mySystemInterface.h>
#include <myRenderInterface.h>

void main() {
	myRenderInterface myRI(1600, 900, &std::string("Test libRocket"));

	//TODO: create OpenGL window here, before SetRenderInterface call
	//Rml::SetRenderInterface((Rml::RenderInterface*) & myRI);
	Rml::SetRenderInterface(& myRI);
	mySystemInterface mySI;

	//Rml::SetSystemInterface((Rml::SystemInterface*) & mySI);
	Rml::SetSystemInterface(& mySI);


	Rml::Initialise();

	Rml::String name("default");

	Rml::Context* context = Rml::CreateContext("default", Rml::Vector2i(1600, 900));

	myRI.context = context;

	//const Rml::String fonts = "assets/";

	myRI.LoadFonts("C:/Users/vasilenk/source/repos/myRmlUi/x64/Debug/assets/");

	Rml::ElementDocument* document = context->LoadDocument("C:/Users/vasilenk/source/repos/myRmlUi/x64/Debug/assets/hello_world_cust.rml");
	//Rml::ElementDocument* document = context->LoadDocument("assets/Sample.rml");

	if (document != NULL) {

		document->Show();
		
		Rml::ElementFormControlInput* text_input = (Rml::ElementFormControlInput*)document->GetElementById("text_input");
		text_input->SetValue("other text");

		//Rml::Element* text_input = document->GetElementById("text_input");
		//
		//el->SetInnerRML("Yes, this is dog.<p>This is sparta");

	} else
		std::cout << "failed document loading\n" << mySI.GetElapsedTime() << std::endl;

	std::cout << "time passed:" << mySI.GetElapsedTime() << std::endl;
	int passedSeconds = 0;
	float curr;
	float Passed;
	bool newTextShown = false;
	while (!glfwWindowShouldClose(myRI.getScreen())) {

		context->Update();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		context->Render();

		glfwSwapBuffers(myRI.getScreen());
		RMLUI_FrameMark;

		glfwPollEvents();
	}

	glfwTerminate();
}