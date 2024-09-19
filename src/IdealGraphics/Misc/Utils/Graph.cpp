#include "Graph.h"

void Ideal::BezierCurve::AddControlPoint(Ideal::Point p)
{
	m_points.push_back(p);
}

Ideal::Point Ideal::BezierCurve::GetPoint(float t)
{
	//int32 key = static_cast<int32>(t * 100);
	//if (m_cache.find(key) != m_cache.end())
	//{
	//	return m_cache[key];
	//}

	Ideal::Point point = Bezier(t);
	//m_cache[key] = point;
	return point;
}

Ideal::Point Ideal::BezierCurve::Bezier(float t)
{
	int n = m_points.size();

	if (n == 0) return Point(0, 0);

	if (n < 2) return m_points[0];

	std::vector<Ideal::Point> tempPoints = m_points;

	for (int k = 1; k < n; ++k) {
		for (int i = 0; i < n - k; ++i) {
			tempPoints[i] = tempPoints[i] * (1 - t) + tempPoints[i + 1] * t;
		}
	}

	// 최종적으로 얻은 곡선 위의 점 반환
	return tempPoints[0];
}
