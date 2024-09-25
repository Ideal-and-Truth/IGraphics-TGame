#pragma once
#include "Core/Core.h"

//namespace Ideal
//{
struct Bounds
{
	Bounds()
	{
		m_Center = Vector3(0, 0, 0);
		m_Extents = Vector3(0, 0, 0);
	}
	Bounds(Vector3 center, Vector3 size)
	{
		m_Center = center;
		m_Extents = size * 0.5f;
	}

	Vector3 GetSize() { return m_Extents * 2.f; }
	void SetSize(Vector3 Value) { m_Extents = Value * 0.5f; }

	Vector3 GetCenter() { return m_Center; }
	void SetCenter(Vector3 Center) { m_Center = Center; }

	Vector3 GetMin()
	{
		m_min = m_Center - m_Extents;
		return m_min;
	}
	void SetMin(Vector3 min) { SetMinMax(min, m_max); }

	Vector3 GetMax()
	{
		m_max = m_Center + m_Extents;
		return m_max;
	}
	void SetMax(Vector3 max) { SetMinMax(m_min, max); }

	bool Contains(Vector3 Point)
	{
		Vector3 minBounds = m_Center - m_Extents;
		Vector3 maxBounds = m_Center + m_Extents;
		bool b = (
			Point.x >= minBounds.x && Point.x <= maxBounds.x &&
			Point.y >= minBounds.y && Point.y <= maxBounds.y &&
			Point.z >= minBounds.z && Point.z <= maxBounds.z
			);
		return b;
	}

	void Encapsulate(Vector3 point)
	{
		auto min = Vector3::Min(GetMin(), point);
		auto max = Vector3::Max(GetMax(), point);
		SetMinMax(min, max);
	}

	void SetMinMax(Vector3 min, Vector3 max)
	{
		m_Extents = (max - min) * 0.5f;
		m_Center = min + m_Extents;
	}

private:
	Vector3 m_Center;
	Vector3 m_Extents;
	Vector3 m_min;
	Vector3 m_max;
};

template <typename T>
class OctreeNode : public std::enable_shared_from_this<OctreeNode<T>>
{
public:
	OctreeNode()
	{

	}

	OctreeNode(Bounds Bound, float MinNodeSize)
	{
		m_nodeBounds = Bound;
		m_minSize = MinNodeSize;

		float quarter = m_nodeBounds.GetSize().y / 4.0f;
		float childLength = m_nodeBounds.GetSize().y / 2.0f;
		Vector3 childSize = Vector3(childLength, childLength, childLength);
		m_childBounds.resize(8);
		m_childBounds[0] = Bounds(m_nodeBounds.GetCenter() + Vector3(-quarter, quarter, -quarter), childSize);
		m_childBounds[1] = Bounds(m_nodeBounds.GetCenter() + Vector3(quarter, quarter, -quarter), childSize);
		m_childBounds[2] = Bounds(m_nodeBounds.GetCenter() + Vector3(-quarter, quarter, quarter), childSize);
		m_childBounds[3] = Bounds(m_nodeBounds.GetCenter() + Vector3(quarter, quarter, quarter), childSize);
		m_childBounds[4] = Bounds(m_nodeBounds.GetCenter() + Vector3(-quarter, -quarter, -quarter), childSize);
		m_childBounds[5] = Bounds(m_nodeBounds.GetCenter() + Vector3(quarter, -quarter, -quarter), childSize);
		m_childBounds[6] = Bounds(m_nodeBounds.GetCenter() + Vector3(-quarter, -quarter, quarter), childSize);
		m_childBounds[7] = Bounds(m_nodeBounds.GetCenter() + Vector3(quarter, -quarter, quarter), childSize);
	}

	void DivideAndAdd(T Object, Vector3 Position)
	{
		m_objects.push_back(Object);
		if (m_nodeBounds.GetSize().y <= m_minSize)
		{
			m_isFinalNode = true;
			return;
		}
		if (m_childOctreeNodes.size() == 0)
		{
			m_childOctreeNodes.resize(8);
		}
		bool dividing = false;
		for (int i = 0; i < 8; ++i)
		{
			if (m_childOctreeNodes[i] == nullptr)
			{
				m_childOctreeNodes[i] = std::make_shared<OctreeNode<T>>(m_childBounds[i], m_minSize);
			}

			if (m_childBounds[i].Contains(Position))
			{
				dividing = true;
				m_childOctreeNodes[i]->DivideAndAdd(Object, Position);
			}
		}
		if (dividing = false)
		{
			m_childOctreeNodes.clear();
		}
	}
	bool IsFinalNode()
	{
		return m_isFinalNode;
	}
	// 모든 노드를 순회
	template <typename Func>
	void ForeachNodeInternal(Func func)
	{
		// 일단 나부터
		func(shared_from_this());
		// 그리고 자식노드도 다시
		for (auto& c : m_childOctreeNodes)
		{
			c->ForeachNodeInternal(func);
		}
	}

private:

	Bounds m_nodeBounds;
	std::vector<Bounds> m_childBounds;
	std::vector<std::shared_ptr<OctreeNode<T>>> m_childOctreeNodes;
	float m_minSize;
	std::vector<T> m_objects;
	bool m_isFinalNode = false;
};

template <typename T>
class Octree
{
	template <typename T>
	struct Object
	{
		T object;
		Vector3 pos;
	};

public:

	void AddObject(T object, Vector3 Position)
	{
		m_bounds.Encapsulate(Position);
		Object<T> obj;
		obj.object = object;
		obj.pos = Position;
		m_objects.push_back(obj);
	}

	void Bake(float minNodeSize)
	{
		float maxSize = max(m_bounds.GetSize().x, max(m_bounds.GetSize().y, m_bounds.GetSize().z));

		Vector3 sizeVector = Vector3(maxSize, maxSize, maxSize) * 0.5f;
		m_bounds.SetMinMax(m_bounds.GetCenter() - sizeVector, m_bounds.GetCenter() + sizeVector);
		m_rootNode = std::make_shared<OctreeNode<T>>(m_bounds, minNodeSize);

		for (auto& o : m_objects)
		{
			m_rootNode->DivideAndAdd(o.object, o.pos);
		}
	}

	void GetFinalNodes(std::vector<std::shared_ptr<OctreeNode<T>>>& nodes)
	{
		m_rootNode->ForeachNodeInternal([&](std::shared_ptr<OctreeNode<T>> Node) {if (Node->IsFinalNode()) nodes.push_back(Node); });
		//m_rootNode->Foreac ([&](std::shared_ptr<OctreeNode<T>> Node) {if (Node->IsFinalNode()) nodes.push_back(Node); });

		// TODO : 자식
	}

private:
	Bounds m_bounds;
	std::vector<Object<T>> m_objects;
	std::shared_ptr<OctreeNode<T>> m_rootNode;
};
//}
