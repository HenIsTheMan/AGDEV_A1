#pragma once

enum struct ColliderType: int{
	Box,
	Sphere,
	Amt
};

class Collider{ //Abstract class
	friend class Node;
	friend class ColliderManager;
public:
	virtual ~Collider() = default;

	virtual void Reset() = 0;

	///Getter
	ColliderType GetType() const;
protected:
	Collider();
	Collider(const ColliderType type);

	ColliderType type;
};