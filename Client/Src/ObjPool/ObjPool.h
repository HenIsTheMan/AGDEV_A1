#pragma once

#include <vector>

template <class T>
class ObjPool final: public Singleton<ObjPool<T>>{
	friend Singleton<ObjPool<T>>;
public:
	~ObjPool<T>();

	void Init(const size_t& inactiveSize, const size_t& activeSize);

	T* ActivateObj();
	void DeactivateObj(const T* const obj);
private:
	static std::vector<T*> activeObjs;
	static std::vector<T*> inactiveObjs;

	ObjPool<T>() = default;
};

#include "ObjPool.inl"