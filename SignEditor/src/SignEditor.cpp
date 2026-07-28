#include "EditorLayer.h"
#include "Sign/EntryPoint.h"

namespace Sign {
	class Editor : public Application {
	public:
		Editor(const ApplicationSpecifications& specs)
			: Application(specs)
		{

			PushLayer(new EditorLayer);
		}
	};

	Application* CreateApplication() {
		ApplicationSpecifications specifications = {};
		specifications.name = "Sign Engine";
		specifications.WindowSpec.Width = 1600;
		specifications.WindowSpec.Height = 900;
		return new Editor(specifications);
	}
}