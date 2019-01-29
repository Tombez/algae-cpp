#include "./Geometry.hpp"

#include <cstdint>
#include <utility>
#include <vector>
#include <cassert>

template<class T>
class QuadBranch : public Square {
public:
	std::vector<T*> objects;
	QuadBranch* children[4] = {nullptr, nullptr, nullptr, nullptr};
	bool hasSplit;
	QuadBranch() {}
	QuadBranch(float x, float y, float s) : Square(x, y, s), hasSplit(false) {}
	QuadBranch(const QuadBranch<T>&) = delete;
	QuadBranch(QuadBranch<T>& rhs) {
		x = rhs.x;
		y = rhs.y;
		s = rhs.s;
		hasSplit = rhs.hasSplit;
		if (rhs.hasSplit) {
			for (uint8_t i = 0; i < 4; ++i) { // compiler unroll?
				children[i] = rhs.children[i];
				rhs.children[i] = nullptr;
			}
			rhs.hasSplit = false;
		}
	};
	~QuadBranch() {
		if (hasSplit) {
			for (uint8_t i = 0; i < 4; ++i) { // compiler unroll?
				delete children[i];
			}
			hasSplit = false;
		}
	}
	QuadBranch<T>* findQB(T* c) {
		assert(hasSplit);
		float hs = s / 2;
		const bool col = c->x > x + hs;
		const bool row = c->y > y + hs;
		auto child = children[col | row << 1];
		if (child->containsCircle(*c)) {
			return child;
		}
		return this;
	};
};

template<class T>
class LooseQuadTree : public QuadBranch<T> {
public:
	uint8_t maxObjects;
	LooseQuadTree(float x, float y, float s, uint8_t maxObjects) :
		QuadBranch<T>(x - s / 2, y - s / 2, s * 2), maxObjects(maxObjects) {}
	void insertCircle(T* c) {
		typedef std::pair<T*, QuadBranch<T>*> PairTQB;
		std::vector<PairTQB> stack;
		stack.emplace_back(c, this);
		while (stack.size() != 0) {
			PairTQB pair = stack.back();
			stack.pop_back();
			T* c = pair.first;
			QuadBranch<T>* qb = pair.second;
			while (qb->hasSplit) {
				QuadBranch<T>* nqb = qb->findQB(c);
				if (nqb == qb) {
					break;
				}
				qb = nqb;
			}
			qb->objects.push_back(c);
			if (qb->objects.size() > maxObjects &&
				!qb->hasSplit && qb->s > 1.0)
			{
				float hs = qb->s / 2;
				float qs = qb->s / 4;
				float es = qb->s / 8;
				for (uint8_t y = 0; y < 2; ++y) { // compiler unroll?
					for (uint8_t x = 0; x < 2; ++x) { // compiler unroll?
						const uint8_t i = x + y * 2;
						const float px = qb->x + x * qs + es;
						const float py = qb->y + y * qs + es;
						qb->children[i] = new QuadBranch<T>(px, py, hs);
					}
				}
				qb->hasSplit = true;
				uint8_t n = 0;
				for (uint8_t i = 0; i < qb->objects.size(); ++i) {
					T* c = qb->objects[i];
					QuadBranch<T>* nqb = qb->findQB(c);
					if (nqb == qb) {
						qb->objects[n++] = c;
					} else {
						stack.emplace_back(c, nqb);
					}
				}
				qb->objects.resize(n);
			}
		}
	}
	template<typename CB>
	std::vector<T*> getVerletList(AABB& aabb, CB shouldInclude) {
		std::vector<T*> verletList;
		std::vector<QuadBranch<T>*> qbs = {this};
		while (qbs.size()) {
			QuadBranch<T>* qb = qbs.back();
			qbs.pop_back();
			for (uint8_t i = 0; i < qb->objects.size(); ++i) {
				T* cur = qb->objects[i];
				if (shouldInclude(cur)) {
					verletList.push_back(cur);
				}
			}
			if (qb->hasSplit) {
				for (uint8_t i = 0; i < 4; ++i) { // compiler unroll?
					qbs.push_back(qb->children[i]);
				}
			}
		}
		return std::move(verletList);
	}
	std::vector<T*> getVerletList(AABB& aabb) {
		return this->getVerletList(aabb, [](T* arg)->bool{return true;});
	}
	void clear() {
		if (this->hasSplit) {
			for (uint8_t i = 0; i < 4; ++i) { // compiler unroll?
				delete this->children[i];
			}
			this->hasSplit = false;
		}
		this->objects.clear();
	}
};
