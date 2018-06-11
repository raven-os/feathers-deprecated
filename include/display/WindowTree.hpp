#pragma once

#include <iterator>

#include <claws/tagged_data.hpp>
#include <claws/iterator_util.hpp>

#include <array>
#include <vector>

namespace display
{
  class WindowTree
  {
    struct Rect
    {
      std::array<uint16_t, 2u> position;
      std::array<uint16_t, 2u> size;
    };
    
    struct WindowData
    {
      Rect rect;
      bool clamp;
      bool invertClamp;
    };

    std::vector<WindowData> windows;

    struct WindowNodeTag;
    struct WindowDataTag;

    using WindowNodeIndex = claws::tagged_data<uint16_t, uint16_t, WindowNodeTag>;
    using WindowDataIndex = claws::tagged_data<uint16_t, uint16_t, WindowDataTag>;

    static constexpr WindowDataIndex noData{uint16_t(-1u)};
    static constexpr WindowNodeIndex nullNode{uint16_t(-1u)};

    struct WindowNode
    {
      WindowNodeIndex parent;
      WindowNodeIndex firstChild;
      WindowNodeIndex nextSibling;
      WindowDataIndex dataIndex;

      constexpr bool hasData() const noexcept
      {
	return dataIndex == noData;
      }
    };

    WindowNodeIndex freeList;
    std::vector<WindowNode> nodes;

    WindowNode &getNode(WindowNodeIndex nodeIndex) noexcept
    {
      return nodes[nodeIndex.data];
    }

    WindowNode const &getNode(WindowNodeIndex nodeIndex) const noexcept
    {
      return const_cast<WindowTree &>(*this).getNode(nodeIndex);
    }
  public:

    WindowTree()
      : freeList(nullNode)
    {
      nodes.emplace_back(WindowNode{nullNode, nullNode, nullNode, noData});
    }

    WindowNodeIndex getRootIndex() const noexcept
    {
      return WindowNodeIndex{0};
    }

    struct Iterator
    {
      WindowTree const *windowTree;
      WindowNodeIndex nodeIndex;

      Iterator &operator++() noexcept
      {
	nodeIndex = windowTree->getSibling(nodeIndex);
	return *this;
      }

      constexpr bool operator==(Iterator const &other) const noexcept
      {
	return nodeIndex == other.nodeIndex && windowTree == other.windowTree;
      }

      constexpr bool operator!=(Iterator const &other) const noexcept
      {
	return !(*this == other);
      }

      constexpr WindowNodeIndex operator*() const noexcept
      {
	return nodeIndex;
      }
    };

    WindowNodeIndex getParent(WindowNodeIndex nodeIndex) const noexcept
    {
      return getNode(nodeIndex).parent;
    }

    WindowNodeIndex getFirstChild(WindowNodeIndex nodeIndex) const noexcept
    {
      return getNode(nodeIndex).firstChild;
    }

    WindowNodeIndex getSibling(WindowNodeIndex nodeIndex) const noexcept
    {
      return getNode(nodeIndex).nextSibling;
    }

    claws::iterator_pair<Iterator, Iterator> getChildren(WindowNodeIndex nodeIndex) const noexcept
    {
      return {Iterator{this, getFirstChild(nodeIndex)}, Iterator{this, nullNode}};
    }
    
    WindowData &getData(WindowNodeIndex nodeIndex) noexcept
    {
      return windows[getNode(nodeIndex).dataIndex.data];
    }

    WindowData const &getData(WindowNodeIndex nodeIndex) const noexcept
    {
      return const_cast<WindowTree &>(*this).getData(nodeIndex);
    }

    WindowNodeIndex allocateIndex()
    {
      if (freeList == nullNode)
	{
	  WindowNodeIndex result(static_cast<uint16_t>(nodes.size()));

	  nodes.emplace_back();
	  return result;
	}
      else
	{
	  WindowNodeIndex result(freeList);

	  freeList = getSibling(freeList);
	  return freeList;
	}
    }

    void removeIndex(WindowNodeIndex index)
    {
      WindowNodeIndex child(getFirstChild(getParent(index)));

      if (child == index)
	getNode(getParent(index)).firstChild = nullNode;
      else
	{
	  while (getSibling(child) != index)
	    child = getSibling(child);
	  getNode(child).nextSibling = getSibling(index);
	}
      getNode(index).nextSibling = freeList;
      freeList = index;
    }

    WindowNodeIndex addChild(WindowNodeIndex parent)
    {
      WindowNodeIndex result(allocateIndex());

      getNode(result).parent = parent;
      getNode(result).nextSibling = getFirstChild(parent);
      getNode(result).firstChild = nullNode;
      getNode(result).dataIndex = noData;
      getNode(parent).firstChild = result;
      return result;
    }
  };
}
