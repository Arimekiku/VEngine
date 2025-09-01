#include "Engine/Renderer.h"

int main()
{
	VEngine::Renderer renderer;
	renderer.Initialize();

	while (renderer.IsRunning())
	{
		renderer.Update();
	}

	renderer.Shutdown();
}
