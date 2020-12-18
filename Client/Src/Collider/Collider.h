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

	///Getters
	bool GetActive() const;
	ColliderType GetType() const;
protected:
	Collider();
	Collider(const ColliderType type);

	bool active;
	ColliderType type;
};