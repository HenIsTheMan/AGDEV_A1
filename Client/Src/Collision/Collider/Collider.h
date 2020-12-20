#pragma once

enum struct ColliderType: int{
	Box,
	Sphere,
	Amt
};

class Collider{ //Abstract class
	friend class ColliderManager;
public:
	virtual ~Collider() = default;

	///Getter
	ColliderType GetType() const;
protected:
	Collider();
	Collider(const ColliderType type);

	ColliderType type;
};