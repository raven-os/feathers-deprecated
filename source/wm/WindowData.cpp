#include "wm/WindowData.hpp"

namespace wm
{
  void Container::recalculateChildren(WindowNodeIndex index, WindowTree &windowTree, WindowData const &windowData)
  {
    std::visit([index, &windowTree, &windowData](auto &data)
	       {
		 data.recalculateChildren(index, windowTree, windowData);
	       }, data);
  }

  void WindowData::recalculateChildren(WindowNodeIndex index, WindowTree &windowTree)
  {
    std::get<Container>(data).recalculateChildren(index, windowTree, *this);
  }
}
