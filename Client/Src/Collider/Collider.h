#pragma once

enum struct ColliderType: int{
	Box,
	Sphere,
	Amt
};

class Collider{ //Abstract class
public:
	///Getter
	ColliderType GetType () const;

	///Setter
	void SetType(const ColliderType type);
protected:
	Collider();
	Collider(const ColliderType type);

	ColliderType type;
};