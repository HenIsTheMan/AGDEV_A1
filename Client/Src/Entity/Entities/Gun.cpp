#include "Gun.h"

extern float dt;

Gun::Gun():
	canShoot(true),
	reloadBT(0.f),
	reloadTime(0.f),
	shotCooldownTime(0.f),
	loadedBullets(0),
	maxLoadedBullets(0),
	unloadedBullets(0),
	maxUnloadedBullets(0),
	entityFactory(EntityManager::GetObjPtr()->RetrieveEntityFactory()),
	reloading(false)
{
}

const bool& Gun::GetCanShoot() const{
	return canShoot;
}

const float& Gun::GetReloadBT() const{
	return reloadBT;
}

const float& Gun::GetReloadTime() const{
	return reloadTime;
}

const float& Gun::GetShotCooldownTime() const{
	return shotCooldownTime;
}

const int& Gun::GetLoadedBullets() const{
	return loadedBullets;
}

const int& Gun::GetMaxLoadedBullets() const{
	return maxLoadedBullets;
}

const int& Gun::GetUnloadedBullets() const{
	return unloadedBullets;
}

const int& Gun::GetMaxUnloadedBullets() const{
	return maxUnloadedBullets;
}

void Gun::SetCanShoot(const bool& canShoot){
	this->canShoot = canShoot;
}

void Gun::SetReloadBT(const float& reloadBT){
	this->reloadBT = reloadBT;
}

void Gun::SetReloadTime(const float& reloadTime){
	this->reloadTime = reloadTime;
}

void Gun::SetShotCooldownTime(const float& shotCooldownTime){
	this->shotCooldownTime = shotCooldownTime;
}

void Gun::SetLoadedBullets(const int& loadedBullets){
	this->loadedBullets = loadedBullets;
}

void Gun::SetMaxLoadedBullets(const int& maxLoadedBullets){
	this->maxLoadedBullets = maxLoadedBullets;
}

void Gun::SetUnloadedBullets(const int& unloadedBullets){
	this->unloadedBullets = unloadedBullets;
}

void Gun::SetMaxUnloadedBullets(const int& maxUnloadedBullets){
	this->maxUnloadedBullets = maxUnloadedBullets;
}

void Gun::Update(){
	if(reloading){
		if(reloadBT <= reloadTime){
			reloadBT += dt;
		} else{
			canShoot = true;
			loadedBullets += unloadedBullets;
			unloadedBullets = loadedBullets < maxLoadedBullets ? 0 : loadedBullets - maxLoadedBullets;
			loadedBullets = std::min(maxLoadedBullets, loadedBullets);
			reloading = false;
		}
	}
}

void Gun::Reload(ISoundEngine* const& soundEngine){
	if(loadedBullets == maxLoadedBullets){
		return;
	} else if(!reloading){
		soundEngine->play2D("Audio/Sounds/Reload.wav", false);
		canShoot = false;
		reloadBT = 0.f;
		reloading = true;
	}
}