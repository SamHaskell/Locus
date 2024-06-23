#pragma once

#include "Core/DisplayManager.hpp"

namespace Locus
{
	class LSDLDisplayManager : public DisplayManager
	{
	public:
		WindowHandle CreateWindow() override;
		void DestroyWindow(WindowHandle Window) override;
	private:
	};
}