#pragma once

#include <vector>

#include "Structures.h"

constexpr int DIMENSIONS = 3;

class KdTree
{
public:
	KdTree(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);
	KdTree(float* vertices, unsigned int vertexCount);
	KdTree(std::vector<KdStructs::Point*> points);
	~KdTree();

	void raycast(KdStructs::Ray ray, KdStructs::RayHit*& hit);
	std::vector<KdStructs::Node*> getNodes();

	void print();
	void printStatistics();

private:

	std::vector<KdStructs::Point*> getPointList(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);
	std::vector<KdStructs::Point*> getPointList(float* vertices, unsigned int vertexCount);
	KdStructs::Node* createKdTree(std::vector<KdStructs::Point*> points, int depth, KdStructs::Vector max, KdStructs::Vector min);
	void findIntersection(KdStructs::Node* node, KdStructs::Ray ray, KdStructs::RayHit*& hit);
	float rayIntersectionWithTriangle(KdStructs::Triangle* triangle, KdStructs::Ray ray);

	inline auto getComparatorForAxis(int axis) const
	{ 
		return [axis](KdStructs::Point* p1, KdStructs::Point* p2)
		{
			return p1->pos[axis] < p2->pos[axis]; 
		}; 
	}

	inline KdStructs::Point* findPoint(KdStructs::Point point, std::vector<KdStructs::Point*> points) {
		for (KdStructs::Point* currentPoint : points)
			if (currentPoint->pos == point.pos)
				return currentPoint;
		return nullptr;
	}

	KdStructs::Node* root;
	// Used for clean up.
	std::vector<KdStructs::Triangle*> triangles;

	unsigned int checkCache = 0;
};

