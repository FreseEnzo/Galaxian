#pragma once

enum EntityType
{
	//ENTIDADES
	player,
	weapon,
	enemyWeapon,
	enemyMasterWeapon,
	enemy,
	enemyMaster,
	block,
	flyby,
	flybyWeapon
};

class Entity
{
public:
	Entity() { };
	~Entity() { };

public:
	sf::Sprite m_sprite;
	sf::Vector2u m_size;
	sf::Vector2f m_position;
	EntityType m_type;
	bool m_enabled = true;
	

	//PROPRIEDADES DO INIMIGO
	bool m_bLeftToRight = true;
	int m_times = 0;
	bool m_flyby = false;
	bool m_flybydead = false;
};

