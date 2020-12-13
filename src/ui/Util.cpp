#include "Util.hpp"

namespace gui
{
	ImVec4 rgba(int r, int g, int b, float alpha)
	{
		return ImVec4((float)r / 255.f, (float)g / 255.f, (float)b / 255.f, alpha);
	}
}