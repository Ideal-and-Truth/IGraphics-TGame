#pragma once
#include "GraphicsEngine/public/IGraph.h"
//#include "IGraph.h"
#include "Core/Core.h"
namespace Ideal
{
	class BezierCurve : public Ideal::IBezierCurve
	{
	public:

	public:
		virtual void AddControlPoint(Point p) override;
		virtual Point GetPoint(float t)override;

	private:
		virtual Point Bezier(float t) override;

	private:
		std::vector<Point> m_points;
		std::unordered_map<int32, Point> m_cache;
	};
}