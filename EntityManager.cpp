#include "pch.h"
#include "EntityManager.h"

std::vector<std::shared_ptr<Entity>> EntityManager::m_Entities;

EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}

//RETORNA O OBJETO PLAYER
std::shared_ptr<Entity> EntityManager::GetPlayer()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			
			continue;
		}

		if (entity->m_type == EntityType::player)
		{
			return entity;
		}
	}

	return nullptr;
}

//RETORNA O OBJETO ENEMYMASTER
std::shared_ptr<Entity> EntityManager::GetEnemyMaster()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_type == EntityType::enemyMaster)
		{
			return entity;
		}
	}

	return nullptr;
}

//RETORNA O OBJETO FLYBY (RASANTE)
std::shared_ptr<Entity> EntityManager::GetFlyby()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{

			continue;
		}

		if (entity->m_type == EntityType::flyby)
		{
			return entity;
		}
	}

	return nullptr;
}

