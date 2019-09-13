// playground.cpp : Defines the entry vec3 for the console application.
//

#include "stdafx.h"
#define GLM_SWIZZLE 


#include <iostream>
#include <iomanip>
#include <random>
#include <stack>
#include <set>
#include <limits>
//#include <ncl/space_partition/bsp_tree.h>
#include <ncl/geom/Plane.h>
#include <ncl/geom/aabb2.h>
#include <glm/vec_util.h>
#include "mathlab.h"
#include "macbeth.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace glm;
using namespace std;
using namespace ncl::geom;
/*
ostream& operator<<(ostream& out, const vec3& v) {
	out << std::setprecision(3);
	out << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return out;
}
*/
/*
ostream& operator<<(ostream& out, const Vector3& v) {
	out << std::setprecision(3);
	out << "[ " << v.x << ", " << v.y << ", " << v.z << ", " << " ]";
	return out;
}*/

float y(float y0, float t, float v) {
	return y0 + v * t + 0.5 * -10 * t * t;
}

void eular(float p0, float v0, float a0, float dt, int iterations) {
	float p1 = 0;
	float v1 = 0;
	cout << "Euler Integration:" << endl;
	for (int i = 0; i < iterations; i++) {
		p1 = p0 + dt * v0;
		v1 = v0 + dt * a0;
		cout << i * dt << "\t" << p1 << "\t" << v1 << endl;
		p0 = p1;
		v0 = v1;

	}
}

void symplecticEular(float p0, float v0, float a0, float dt, int iterations) {
	float p1 = 0;
	float v1 = 0;
	cout << "Symplectic Euler Integration:" << endl;
	for (int i = 0; i < iterations; i++) {
		v1 = v0 + a0 * dt;
		p1 = p0 + v1 * dt;
		cout << i * dt << "\t" << p1 << "\t" << v1 << endl;
		p0 = p1;
		v0 = v1;

	}
}

void varlet(float p0, float v0, float a0, float dt, int iterations) {
	float p_1 = 495;
	float p1 = 0;
	float v1 = 0;
	cout << "varlet Euler Integration:" << endl;
	for (int i = 0; i < iterations; i++) {
		p1 = 2 * p0 - p_1 + dt * dt * a0;
		v1 = ((p1 - p0) - (p0 - p_1)) / (dt);
		//a0 = v1 / dt;

		cout << i * dt << "\t" << p1 << "\t" << v1 << endl;

		p_1 = p0;
		p0 = p1;

	}
}

float f(float x) {
	return 1 + 1 / x;
}

int IntersectSegmentTriangle(vec3 p, vec3 q, vec3 a, vec3 b, vec3 c,
	float &u, float &v, float &w, float &t)
{
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 qp = p - q;

	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	vec3 n = cross(ab, ac);

	// Compute denominator d. If d <= 0, segment is parallel to or vec3s
	// away from triangle, so exit early
	float d = dot(qp, n);
	cout << "id: " << d << endl;
	if (d <= 0.0f) return 0;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	vec3 ap = p - a;
	t = dot(ap, n);
	if (t < 0.0f) return 0;
	if (t > d) return 0; // For segment; exclude this code line for a ray test

						 // Compute barycentric coordinate components and test if within bounds
	vec3 e = cross(qp, ap);
	v = dot(ac, e);
	if (v < 0.0f || v > d) return 0;
	w = -dot(ab, e);
	if (w < 0.0f || v + w > d) return 0;

	// Segment/ray intersects triangle. Perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	return 1;
}

bool TriangleRayIntersect(vec3 a, vec3 b, vec3 c, vec3 p, vec3 q, bool lineTest,  float& u,  float& v, float& w,  float& t) {
	vec3 ba = b - a;
	vec3 ca = c - a;
	vec3 pa = p - a;
	vec3 pq = p - q;

	vec3 n = cross(ba, ca);
	float d = dot(pq, n);

	if (d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it

	t = dot(pa, n);

	if (t < 0) return false;     // ray invariant t >= 0
	if (lineTest && t > d) return false; // line invariant 0 < t < 1

	vec3 e = cross(pq, pa);

	v = dot(e, ca);
	if (v < 0.0f || v > d) return false;

	w = -dot(e, ba);
	if (w < 0.0f || (v + w) > d) return false;

	float ood = 1.0 / d;

	t *= ood;
	v *= ood;
	w *= ood;
	u = 1 - v - w;

	return true;
}

bool TriangleRayIntersect2(vec3 a, vec3 b, vec3 c, vec3 p, vec3 q, bool lineTest, float& u, float& v, float& w, float& t) {

	vec3 ba = b - a;

	vec3 ca = c - a;

	vec3 pa = p - a;

	vec3 pq = p - q;

	vec3 n = cross(ba, ca);

	float d = dot(pq, n);

	if (d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it

	t = dot(pa, n);

	if (t < 0) return false;     // ray invariant t >= 0

	if (lineTest && t > d) return false; // line invariant 0 < t < 1

	vec3 e = cross(pq, pa);

	v = dot(e, ca);

	if (v < 0.0f || v > d) return false;

	w = -dot(e, ba);

	if (w < 0.0f || (v + w) > d) return false;

	float ood = 1.0 / d;

	t *= ood;

	v *= ood;

	w *= ood;

	u = 1 - v - w;

	return true;

}

float halfSize = 1;

vec3 positions[24] = {
	// Front
	vec3(-halfSize, -halfSize, halfSize),
	vec3(halfSize, -halfSize, halfSize),//
	vec3(halfSize,  halfSize, halfSize),
	vec3(-halfSize,  halfSize, halfSize),
	// Right
	vec3(halfSize, -halfSize, halfSize),//
	vec3(halfSize, -halfSize, -halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(halfSize,  halfSize, halfSize),
	// Back
	vec3(-halfSize, -halfSize, -halfSize),
	vec3(-halfSize,  halfSize, -halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(halfSize, -halfSize, -halfSize),
	// Left
	vec3(-halfSize, -halfSize, halfSize),
	vec3(-halfSize,  halfSize, halfSize),
	vec3(-halfSize,  halfSize, -halfSize),
	vec3(-halfSize, -halfSize, -halfSize),
	// Bottom
	vec3(-halfSize, -halfSize, halfSize),
	vec3(-halfSize, -halfSize, -halfSize),
	vec3(halfSize, -halfSize, -halfSize),
	vec3(halfSize, -halfSize, halfSize),//
	// Top
	vec3(-halfSize,  halfSize, halfSize),
	vec3(halfSize,  halfSize, halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(-halfSize,  halfSize, -halfSize)
};

unsigned indices[] = {
	0,1,2,0,2,3,
	4,5,6,4,6,7,
	8,9,10,8,10,11,
	12,13,14,12,14,15,
	16,17,18,16,18,19,
	20,21,22,20,22,23
};


template<typename T>
class Foo {
public:
	using Boo = T * ;
	static Foo<T> doSomething(Boo b);
};

template<typename T>
Foo<T> Foo<T>::doSomething(typename Foo<T>::Boo b) {

}

class Node {
public:
//	Node* parent;
	Node* child[2] = { nullptr, nullptr };
	char value;

	bool isLeaf() {
		return child[0] == nullptr && child[1] == nullptr;
	}
};

void insert(Node* parent, Node* node) {
	if (parent->value > node->value) {
		if (parent->child[0] == nullptr) {
			parent->child[0] = node;
		}
		else insert(parent->child[0], node);
	}
	else {
		if (parent->child[1] == nullptr) parent->child[1] = node;
		else insert(parent->child[1], node);
	}
}

void printInOrder(Node* node) {
	if (!node) return;
	printInOrder(node->child[0]);
	cout << node->value << " ";
	printInOrder(node->child[1]);
}

void printInOrderStack(Node* node) {
	stack<Node*> stack;
	Node* curr = node;
	while (curr || !stack.empty()) {
		while (curr) {
			stack.push(curr);
			curr = curr->child[0];
		}

		curr = stack.top();
		stack.pop();
		cout << curr->value << " ";
		curr = curr->child[1];
	}
}

void lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up, Matrix4& matrix) {
	auto m_xAxis = Vector3();
	auto m_yAxis = Vector3();
	auto m_zAxis = Vector3();

	m_zAxis = eye - target;
	m_zAxis.normalize();


	m_xAxis = Vector3::cross(up, m_zAxis);
	m_xAxis.normalize();

	m_yAxis = Vector3::cross(m_zAxis, m_xAxis);
	m_yAxis.normalize();

	matrix[0][0] = m_xAxis.x;
	matrix[1][0] = m_xAxis.y;
	matrix[2][0] = m_xAxis.z;
	matrix[3][0] = -Vector3::dot(m_xAxis, eye);

	matrix[0][1] = m_yAxis.x;
	matrix[1][1] = m_yAxis.y;
	matrix[2][1] = m_yAxis.z;
	matrix[3][1] = -Vector3::dot(m_yAxis, eye);

	matrix[0][2] = m_zAxis.x;
	matrix[1][2] = m_zAxis.y;
	matrix[2][2] = m_zAxis.z;
	matrix[3][2] = -Vector3::dot(m_zAxis, eye);
}

ostream& operator<<(ostream& out, const mat4& m) {
	out << (row(m, 0)) << endl;
	out << (row(m, 1)) << endl;
	out << (row(m, 2)) << endl;
	out << vec4(0, 0, 0, 1) << endl;

	return out;
}

struct Camera {
	vec3 u, v, w, p;
} cam;

struct Ray {
	vec3 o = vec3(0);
	vec3 d = vec3(0, 0, -1);
};

ostream& operator <<(ostream& out, const Ray& ray) {
	cout << "Ray( origin: " << ray.o << ", direction: " << ray.d;
	return out;
}

vec3 dir(vec2 res, float fov, vec2 point, mat3 cam) {
	float w = res.x;
	float h = res.y;
	float x = point.x;
	float y = point.y;
	float a = w / h;
	float f = tan(radians(fov / 2));
	vec3 r = row(cam, 0);
	vec3 u = row(cam, 1);
	vec3 v = row(cam, 2);

	vec3 s = a * f * ((2.f * (x + 0.5f)) / w - 1.f) * r + f * ((2.f * (y + 0.5f)) / h - 1.f) * u + v;
	return normalize(s);
}


mat4 RasterToScreen(float w, float h) {
	mat4 mat = translate(mat4(1), vec3(-1, -1, 0));
	mat = scale(mat, { 2 / w, 2 / h, 1 });
	return mat;
}
/*
mat4 ScreenToRaster(float w, float h) {
	return inverse(RasterToScreen(w, h));
}

mat4 RasterToCamera(float w, float h, mat4 cameraToScreen) {
	auto rasterToScreen = RasterToScreen(w, h);
	return inverse(cameraToScreen) * rasterToScreen;
}*/

Ray& operator*(const mat4 mat, Ray& ray) {
	ray.o = vec3(mat * vec4(ray.o, 1.0f));
	ray.d = mat3(mat) * ray.d;
	return ray;
}

float sphere(vec3 p, float s) {
	return length(p) - s;
}

const vec3 iResolution = vec3(1024, 720, 1);

mat4 rasterToScreen() {
	mat4 m = mat4(1.0);
	m = translate(m, vec3(-1.0, -1.0, 0.0));
	m  = scale(m, vec3(2.0 / iResolution.x, 2.0 / iResolution.y, 1.0));

	return m;
}

vec2 e2PIi(float f, float t) {
	float theta = -glm::two_pi<float>() * f * t;
	return { cos(theta), sin(theta) };
}

float g(float f, float t) {
	return std::cos(glm::two_pi<float>() * f * t);
}

struct stack_t {
	int size;
	int top;
	int data[64];
};

bool empty(stack_t& stack) {
	return stack.size == 0;
}

void push(stack_t& stack, int elm) {
	stack.top = stack.size;
	stack.size += 1;
	stack.data[stack.top] = elm;
}

int pop(stack_t& stack) {
	if (empty(stack)) return -1;
	int elm = stack.data[stack.top];
	stack.size = stack.top;
	stack.top -= 1;
	return elm;
}

int peek(stack_t& stack) {
	if (empty(stack)) return -1;
	return stack.data[stack.top];
}



char tree[9] = { 'X', 'M', 'K', 'E', 'A', 'F', 'D', 'D', 'B' };

int left(int node) {
	return 2 * node + 1;
};

int right(int node) {
	return 2 * node + 2;
}

int parent(int node) {
	return node == 0 ? -1 : (node - 1) / 2;
}

bool isNull(int node) {
	return node < 0 || node >= 9;
}

void traverse(char* tree) {
	stack_t stack;
	stack.size = 0;

	int root = 0;

	do {

		while (!isNull(root)) {
			if (!isNull(right(root))) {
				push(stack, right(root));
			}
			push(stack, root);
			root = left(root);
		}

		root = pop(stack);

		if (!isNull(right(root)) && peek(stack) == right(root)) {
			pop(stack);
			push(stack, root);
			root = right(root);
		}
		else {
			int id = root;
			cout << tree[id] << " ";
			root = -1;
		}

	} while (!empty(stack));
}

struct SurfaceInteration {
	vec3 n;
	vec3 p;
	vec2 uv;
	vec3 dpdu;
	vec3 dpdv;
	vec3 sn;
	vec3 st;
	vec3 sb;
	vec4 color;
	int matId;
	int shape;
	int shapeId;
	float n1;	// index of refraction above surface
	float n2; // index of refration below the surface
};

vec3 faceforward(vec3& N, vec3& I, vec3& Nref) {
	return dot(Nref, I) < 0 ? N : -N;
}


vec3 SpecularTransmission_Sample_f(vec3 wo, vec3& wi, vec2 u, float& pdf, SurfaceInteration interact) {
	bool entering = dot(wo, interact.n) > 0;
	float etaI = entering ? interact.n1 : interact.n2;
	float etaT = entering ? interact.n2 : interact.n1;

	float eta = etaI / etaT;
	vec3 n = interact.n;	// TODO use reflection space normal
	n = faceforward(n, wo, n);

	wi = refract(-wo, n, eta);

	bool totalInternalReflection = isnan(wi) == bvec3(true);
	if (totalInternalReflection) {
		pdf = 0;
		return vec3(0);
	}

	pdf = 1.0;
	float cos0i = dot(wi, n);

	vec3 kt = vec3(1);
	//vec3 rtVal = kt.xyz * (vec3(1) - fresnel(vec3(etaI), vec3(etaT), m.kr.xyz, cos0i, FRESNEL_DIELECTRIC));
	// TODO handle mode
	//return rtVal / abs(cos0i);
	return vec3(1.0);
}



int main()
{
	SurfaceInteration interact;
	interact.n = vec3(0.0162, -0.0010, 0.9999);
	interact.n1 = 1.0;
	interact.n2 = 1.57;
	vec3 wo = vec3(-0.0107, -0.1637, 0.9864);
	vec3 wi;
	float pdf;

	SpecularTransmission_Sample_f(wo, wi, vec2(0), pdf, interact);

	cout << wi << endl;

	cin.get();

    return 0;
}