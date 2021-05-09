#pragma once

#include <iostream>

namespace KdStructs {

	// Because C++ is a procedural programming language
	struct Triangle;


	struct Vector
	{
		Vector(float values[3]) : values(values) {}
		Vector(float x, float y, float z) : values(new float[3]{ x,y,z }) {}
		Vector(const Vector& vector) : values(new float[3]{ vector[0], vector[1], vector[2] }) {}

		~Vector() {
			delete[] values;
			values = nullptr;
		}

		Vector& operator=(const Vector& other) { values[0] = other[0]; values[1] = other[1]; values[2] = other[2]; return *this; }
		float operator[](int i) const { return values[i]; }
		float& operator[](int i) { return values[i]; }
		Vector operator+(const Vector& other) const { return Vector(values[0] + other[0], values[1] + other[1], values[2] + other[2]); }
		Vector operator-(const Vector& other) const { return Vector(values[0] - other[0], values[1] - other[1], values[2] - other[2]); }
		Vector operator*(float scalar) const { return Vector(values[0] * scalar, values[1] * scalar, values[2] * scalar); }
		bool operator==(const Vector& other) const
		{
			return std::fabs(values[0] - other[0]) < EPSILON && std::fabs(values[1] - other[1]) < EPSILON && std::fabs(values[2] - other[2]) < EPSILON;
		}

		Vector cross(const Vector& other) const {
			return Vector(values[1] * other[2] - values[2] * other[1], -(values[0] * other[2] - values[2] * other[0]), values[0] * other[1] - values[1] * other[0]);
		}

		float dot(const Vector& other) const { return values[0] * other[0] + values[1] * other[1] + values[2] * other[2]; }

		void print() { std::cout << "{" << values[0] << "," << values[1] << "," << values[2] << "}" << std::endl; }

		float* values = nullptr;
		const float EPSILON = 0.0001f;
	};

	inline std::ostream& operator<<(std::ostream& str, const Vector& vector) {
		return str << "{" << vector.values[0] << "," << vector.values[1] << "," << vector.values[2] << "}";
	}

	struct Point
	{
		Point(Vector pos, std::vector<Triangle*> triangles) : pos(pos), triangles(triangles) {}
		Point(Vector pos, Triangle* triangle) : pos(pos) { triangles.push_back(triangle); }
		Point(Vector pos) : pos(pos) {}

		bool operator==(const Point* other) const { return pos == other->pos; }
		bool operator==(const Point& other) const { return pos == other.pos; }

		Vector pos;
		// Triangles this point belongs to
		std::vector<Triangle*> triangles;
	};


	struct Triangle
	{
		Triangle(Vector a, Vector b, Vector c) : a(a), b(b), c(c) {}

		Vector a;
		Vector b;
		Vector c;

		unsigned int checkCache = 0;
	};

	/// <summary>
	/// Aka. Partition, Cell
	/// Node storing kd-tree
	/// </summary>
	struct Node
	{
		Node(Point* point, Node* left, Node* right, int axis, Vector max, Vector min) : point(point), left(left), right(right), axis(axis), max(max), min(min) {}

		~Node() {
			if (point) delete point;
			if (left) delete left;
			if (right) delete right;

			point = nullptr;
			left = nullptr;
			right = nullptr;
		}

		// Point of this splitting plane
		Point* point = nullptr;

		// Splitting plane to the left and right
		Node* left = nullptr;
		Node* right = nullptr;

		// Axis in which this splitting plane lies
		int axis = 0;

		// Splitting plane limits for each axis
		// Defines where splitting plane for this node starts and ends
		// 0 -> undefined/endless
		Vector max;
		Vector min;
	};


	struct Ray
	{
		Ray(Vector origin, Vector direction, float distance) : origin(origin), direction(direction), distance(distance) {}

		Vector origin;
		Vector direction;
		float distance = 0;
	};

	struct RayHit
	{
		RayHit(Triangle* triangle, Vector position, float distance) : triangle(triangle), position(position), distance(distance) {}

		Triangle* triangle = nullptr;
		Vector position;
		float distance = 0;
	};
}