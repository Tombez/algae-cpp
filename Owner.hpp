#pragma once

template<class T>
class Owner {
private:
	T* pointer;
public:
	Owner() : pointer(nullptr) {}
	template<class ...Args>
	Owner(Args&& ...args) : {
		pointer = new T(std::forward<Args>(args)...);
	}
	Owner(const Owner<T>&) = delete;
	Owner(Owner<T>& b) {
		pointer = b.pointer;
		b.pointer = nullptr;
	};
	~Owner() {
		delete pointer;
		pointer = nullptr;
	}
	Owner<T>& operator =(const Owner<T>& b) = delete;
	template<class ...Args>
	void operator()(Args&& ...args) {
		delete pointer;
		pointer = new T(std::forward<Args>(args)...);
	}
};
