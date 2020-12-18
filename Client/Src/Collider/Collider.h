#pragma once

enum struct ColliderType: int{
	Box,
	Sphere,
	Amt
};

class Collider{ //Abstract class
public:
	///Getters
	bool GetActive() const;
	ColliderType GetType() const;

	///Setters
	void SetActive(const bool active);
	void SetType(const ColliderType type);
protected:
	Collider();
	Collider(const ColliderType type);

	bool active;
	ColliderType type;
};