#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

//VELOCIDADE DE MOVIMENTO DO PLAYER
const float Game::PlayerSpeed = 200.f;
//TEMPO DO FRAME
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);


Game::Game()
//INICIA O GAME E AS TEXTURAS
	: mWindow(sf::VideoMode(840, 600), "Galaxian", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);
	_TextureExplosion.loadFromFile("Media/Textures/Explosion.png");
	_TextureWeapon.loadFromFile("Media/Textures/SI_Weapon1.png");
	_TextureWeaponEnemy.loadFromFile("Media/Textures/SI_Weapon2.png");
	_TextureWeaponEnemyMaster.loadFromFile("Media/Textures/SI_Weapon3.png");
	mTexture.loadFromFile("Media/Textures/SI_Player.png");
	_TextureEnemyMaster.loadFromFile("Media/Textures/SI_EnemyMaster.png");
	_TextureEnemy.loadFromFile("Media/Textures/SI_Enemy.png");
	mFont.loadFromFile("Media/Sansation.ttf");
	_TextureEnemySecretWeapon.loadFromFile("Media/Textures/SecretWeapon.png");
	_TextureEnemyMasterExplosion.loadFromFile("Media/Textures/SI_EnemyMasterExplosion.png");
	_TexturePlayerExplosion.loadFromFile("Media/Textures/SI_PlayerGameOver.png");
	_TextureWeaponFlyby.loadFromFile("Media/Textures/SI_Weapon2.png");
	InitSprites();
}

void Game::ResetSprites()
{
	//RESETA 
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsEnemyMasterWeaponFired = false;
	_IsFlybyMoving = false;
	_IsFlybyWeaponFired = false;
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}

//FUNÇAO DE RESTART 
void Game::RestartGame()
{
	//VARIAVEL
	bool Init = false;

	//ELIMINA TODOS OS INIMIGOS
	if (Restart == true)
	{
		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
			{
			enemy->m_enabled = false;
			//EVITA QUE OS FLYBY QUE ESTAVAM VIVOS NO RESTART REAPAREÇAM
			if (enemy->m_type != EntityType::enemy)
			{
				enemy->m_flyby = false;
			}
			}
		Restart = false;
		Init = true;
	}
	//COLOCA TODOS OS INIMIGOS EM POSIÇÃO
	if (Restart == false && Init == true)
	{
		InitSprites();
		Init = false;
		mText.setString("");
		Stop = false;
		Win = 0;
		count_restart++;
	}
	
}
void Game::InitSprites()
{
	//INICIA AS SPRITES

	_lives = 5;
	_score = 0;
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsEnemyMasterWeaponFired = false;
	_IsFlybyMoving = false;
	_IsFlybyWeaponFired = false;
	//
	// Player
	//

	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(100.f, 500.f);
	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);

	//
	// Enemy Master
	//

	_EnemyMaster.setTexture(_TextureEnemyMaster);
	_EnemyMaster.setPosition(100.f + 50.f, 1.f);
	std::shared_ptr<Entity> sem = std::make_shared<Entity>();
	sem->m_sprite = _EnemyMaster;
	sem->m_type = EntityType::enemyMaster;
	sem->m_size = _TextureEnemyMaster.getSize();
	sem->m_position = _EnemyMaster.getPosition();
	EntityManager::m_Entities.push_back(sem);

	//
	// Colocando os Inimigos em posição
	//

	for (int i = 0; i < SPRITE_COUNT_X; i++)
	{
		for (int j = 0; j < SPRITE_COUNT_Y; j++)
		{
			_Enemy[i][j].setTexture(_TextureEnemy);
			_Enemy[i][j].setPosition(100.f + 50.f * (i + 1), 30.f + 50.f * (j + 1));

			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = _Enemy[i][j];
			se->m_type = EntityType::enemy;
			se->m_size = _TextureEnemy.getSize();
			se->m_position = _Enemy[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}


	//
	//	VIDAS
	//

	_LivesText.setFillColor(sf::Color::White);
	_LivesText.setFont(mFont);
	_LivesText.setPosition(10.f, 50.f);
	_LivesText.setCharacterSize(20);
	_LivesText.setString(std::to_string(_lives));

	//
	// TEXTO
	//

	_ScoreText.setFillColor(sf::Color::White);
	_ScoreText.setFont(mFont);
	_ScoreText.setPosition(10.f, 100.f);
	_ScoreText.setCharacterSize(20);
	_ScoreText.setString(std::to_string(_score));

}


void Game::run()
{
	//LIDA COM O TEMPO ENTRE EVENTOS

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	//PROCESSA O INPUT DO TECLADO

	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
//ATUALIZA O MOVIMENTO DO PLAYER

	sf::Vector2f movement(0.f, 0.f);
	
	
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::player)
		{
			continue;
		}

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

void Game::render()
{
	//RENDERIZAÇAO

	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);
	mWindow.draw(mText);
	mWindow.draw(_LivesText);
	mWindow.draw(_ScoreText);
	mWindow.draw(_DebugText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	//ATUALIZA ESTATISTICA DE TEMPO EM TEMPO

	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	

	if (mStatisticsUpdateTime >= sf::seconds(0.005f))
	{
		if (_IsGameOver == true)
			return;

		HandleTexts();
		HandleGameOver();
		RestartGame();

		if (Stop == false)
		{
			Flyby();
			HandleEnemyMoves();
			HandleEnemyMasterMove();
			HandleCollisionWeaponEnemy();
			HandleCollisionWeaponPlayer();
			HandleCollisionEnemyMasterWeaponPlayer();
			HandleCollisionWeaponEnemyMaster();
			HandleWeaponMoves();
			HandleEnemyWeaponMoves();
			HandleEnemyMasterWeaponMoves();
			HandleEnemyWeaponFiring();
			HandleEnemyMasterWeaponFiring();
		    HandleFlybyMoves();
			ReachesGround();
			HandleFlybyWeaponFiring();
			HandleFlybyWeaponMoves();
		}
	}
}

void Game::HandleTexts()
{
	//TEXTOS PRINTADOS NA TELA

	std::string lives = "Vidas: " + std::to_string(_lives);
	_LivesText.setString(lives);
	std::string score = "Pontos: " + std::to_string(_score);
	_ScoreText.setString(score);


	return;
}

void Game::HandleCollisionEnemyMasterWeaponPlayer()
{
	//COLISAO SPRITE DA BALA DO PLAYER COM A SPRITE DO BOSS
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyMasterWeaponFired = false; 
			_lives--;
			//break;
			goto end;
		}
	}

end:
	//nop
	return;
}

void Game::HandleEnemyMasterWeaponMoves()
{
	//MOVIMENTO DO TIRO DO BOSS
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y++;

		if (y >= 600)
		{
			entity->m_enabled = false;
			_IsEnemyMasterWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}

}

void Game::HandleEnemyMasterWeaponFiring()
{
	//TIRO DO BOSS
	if (_IsEnemyMasterWeaponFired == true)
		return;

	if (EntityManager::GetEnemyMaster()->m_enabled == false)
		return;

	// ALEATORIEDADE
	int r = rand() % 50;
	if (r != 10)
		return;

	float x, y;
	x = EntityManager::GetEnemyMaster()->m_sprite.getPosition().x;
	y = EntityManager::GetEnemyMaster()->m_sprite.getPosition().y;
	y--;

	std::shared_ptr<Entity> sw = std::make_shared<Entity>();
	sw->m_sprite.setTexture(_TextureWeaponEnemyMaster);

	sw->m_sprite.setPosition(
		x + _TextureEnemyMaster.getSize().x / 2,
		y + _TextureEnemyMaster.getSize().y);
	sw->m_type = EntityType::enemyMasterWeapon;
	sw->m_size = _TextureWeaponEnemyMaster.getSize();
	EntityManager::m_Entities.push_back(sw);

	_IsEnemyMasterWeaponFired = true;
}
void Game::Flyby()
{
	if (_IsFlybyMoving == true)
		return;

	std::vector<std::shared_ptr<Entity>>::reverse_iterator fbm = EntityManager::m_Entities.rbegin();

	//ESCOLHE UM INIMIGO 
	for (; fbm != EntityManager::m_Entities.rend(); fbm++)
	{
		
		std::shared_ptr<Entity> entity = *fbm;

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		// ALEATORIEDADE
		int r = rand() % 15;
		if (r != 10)
			continue;

		//PEGA POSIÇÃO DE UM INIMIGO ALEATORIO
		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;
		
		//CLONANDO INIMIGO
		entity->m_flyby = true;
		entity->m_enabled = false;
		
		//CRIANDO UMA ENTIDADE EnemyFLyby
		std::shared_ptr<Entity> fb = std::make_shared<Entity>();
		fb->m_sprite.setTexture(_TextureEnemy);
		fb->m_sprite.setPosition(
			x + _TextureEnemy.getSize().x / 2,
			y + _TextureEnemy.getSize().y);

		fb->m_sprite.setPosition(
			entity->m_sprite.getPosition().x + _TextureEnemy.getSize().x / 2,
			entity->m_sprite.getPosition().y - 10);

		fb->m_type = EntityType::flyby;
		fb->m_size = _TextureEnemy.getSize();
		EntityManager::m_Entities.push_back(fb);

		_IsFlybyMoving = true;
		break;
	}


	
	

}

void Game::HandleFlybyMoves()
{
	//ENCONTRA O FLYBY NAS ENTITIES
	float dif;
	for (std::shared_ptr<Entity> fly : EntityManager::m_Entities)
	{
		if (fly->m_enabled == false)
		{
			continue;
		}

		if (fly->m_type != EntityType::flyby)
		{
			continue;
		}

		float x, y;
		x = fly->m_sprite.getPosition().x;
		y = fly->m_sprite.getPosition().y;
		y+=0.3f;
		
		//DIRECIONA O FLYBY PARA SEGUIR O PLAYER
		dif = EntityManager::GetPlayer()->m_sprite.getPosition().x - fly->m_sprite.getPosition().x;
		if (dif < 0.f)
		{
			x -= 0.3;
		}
		else
		{
			x+= 0.3;
		}
		if (y >600.f)
		{
			//RETORNA O FLYBY PARA O TOPO
			fly->m_sprite.setPosition(420.f, 1.f);
			secondfly = true;
			continue;
		}
		//INIMIGO VOLTA PRA POSIÇÃO NA ESQUADRA
		if (secondfly == true && y >= 200)
		{	
				fly->m_enabled = false;
				_IsFlybyMoving = false;
				secondfly = false;
				
				//DENTRE TODOS OS INIMIGOS 
				std::vector<std::shared_ptr<Entity>>::reverse_iterator a = EntityManager::m_Entities.rbegin();
				for (; a != EntityManager::m_Entities.rend(); a++)
				{
					
					std::shared_ptr<Entity> entity = *a;

					//INIMIGO DESABILITADO 
					if (entity->m_enabled == true)
					{
						continue;
					}
					//QUE VIROU FLYBY
					if (entity->m_flyby == false)
					{
						continue;
					}
					//QUE NÃO MORREU EM VOO
					if (entity->m_flybydead == true)
					{
						continue;
					}
					//CLASSE FLYBY
					if (entity->m_type != EntityType::enemy)
					{
						continue;
					}
					//RETORNA ELE PARA A ESQUADRA;
					entity->m_enabled = true;
					entity->m_flyby = false;
					
			
				}
				return;
					
		}
		fly->m_sprite.setPosition(x, y);
	}

}

void Game::HandleFlybyWeaponFiring()
{
	if (_IsFlybyMoving == true)
	{

		//ALEATORIEDADE
		int r = rand() % 100;
		if (r != 10)
			return;

		//PEGA POSIÇÃO
		float x, y;
		x = EntityManager::GetFlyby()->m_sprite.getPosition().x;
		y = EntityManager::GetFlyby()->m_sprite.getPosition().y;
		y--;

	//ATIRA
		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeaponEnemyMaster);
		sw->m_sprite.setPosition(
			x + _TextureEnemy.getSize().x / 2,
			y + _TextureEnemy.getSize().y);
		sw->m_type = EntityType::flybyWeapon;
		sw->m_size = _TextureWeaponEnemyMaster.getSize();
		EntityManager::m_Entities.push_back(sw);

	}

}
void Game::HandleFlybyWeaponMoves()
{
	//MOVIMENTO DO TIRO DO INIMIGO NO RASANTE
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::flybyWeapon)
		{
			continue;
		}
		//MOVE O TIRO
		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y++;

		//DESATIVA QUANDO SAI DA TELA
		if (y >= 600)
		{
			entity->m_enabled = false;
			
		}

		entity->m_sprite.setPosition(x, y);
	}
}



void Game::HandleEnemyMasterMove()
{
	//MOVIMENTA O BOSS

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMaster)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		if (entity->m_bLeftToRight == true)
			x = x + 0.5f;
		else
			x = x - 0.5f;

		entity->m_times++;

		if (x >= ((BLOCK_COUNT) * 150) || x <= 150)
		{
			if (entity->m_bLeftToRight == true)
			{
				entity->m_bLeftToRight = false;
				entity->m_times = 0;
			}
			else
			{
				entity->m_bLeftToRight = true;
				entity->m_times = 0;
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}



void Game::HandleCollisionWeaponPlayer()
{
	//ANALISA A INTERSECÇAO DAS SPRITES DOS INIMIGOS COM A SPRITE DA BALA
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyWeaponFired = false;
			_lives--;
			goto end;
		}
	}
	//TIRO DO FLYBY
	for (std::shared_ptr<Entity> flybyweapon : EntityManager::m_Entities)
	{
		if (flybyweapon->m_enabled == false)
		{
			continue;
		}

		if (flybyweapon->m_type != EntityType::flybyWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = flybyweapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			flybyweapon->m_enabled = false;
			
			_lives--;
			goto end;
		}
	}

end:
	
	return;
}

void Game::HandleEnemyWeaponMoves()
{
	//MOVIMENTA A BALA DO INIMIGO
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y += 1.0f;

		if (y >= 600)
		{
			entity->m_enabled = false;
			_IsEnemyWeaponFired = false;
		}
		else
		{
			entity->m_sprite.setPosition(x, y);
		}
	}
}

void Game::HandleEnemyWeaponFiring()
{
	//ESCOLHE O INIMIGO DENTRE OS VIVOS E ATIRA
	if (_IsEnemyWeaponFired == true)
		return;

	std::vector<std::shared_ptr<Entity>>::reverse_iterator rit = EntityManager::m_Entities.rbegin();
	for (; rit != EntityManager::m_Entities.rend(); rit++)
	{
		std::shared_ptr<Entity> entity = *rit;

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		// a little random...
		int r = rand() % 20;
		if (r != 10)
			continue;

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeaponEnemy);
		sw->m_sprite.setPosition(
			x + _TextureWeaponEnemy.getSize().x / 2,
			y + _TextureWeaponEnemy.getSize().y);

		sw->m_sprite.setPosition(
			entity->m_sprite.getPosition().x + _TextureEnemy.getSize().x / 2,
			entity->m_sprite.getPosition().y - 10);

		sw->m_type = EntityType::enemyWeapon;
		sw->m_size = _TextureWeaponEnemy.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsEnemyWeaponFired = true;
		break;
	}
}



void Game::HandleEnemyMoves()
{
	//MOVE OS INIMIGOS EM BLOCOS

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}
		
		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		if (entity->m_bLeftToRight == true)
		{
			x += 0.2f ; // anda pra direita
		}
			
		else
			x -= 0.2f; // anda pra esquerda
		
		entity->m_times += 1;

		if (entity->m_times >= 700) 
		{
			if (entity->m_bLeftToRight == true)
			{
				entity->m_bLeftToRight = false;
				entity->m_times = 0;
				y += 7; // O bloco de inimigos desce
			}
			else
			{
				entity->m_bLeftToRight = true;
				entity->m_times = 0;
				y += 7; // O bloco de inimigos desce
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleWeaponMoves()
{
//MOVIMENTA BALA DO PLAYER

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::weapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;
		
		if (y <= 0)
		{
			entity->m_enabled = false;
			_IsPlayerWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::ReachesGround()
{
	//VE SE ALGUM INIMIGO ATINGIU A ALTURA MINIMA 

	for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
	{
		if (enemy->m_type != EntityType::enemy)
		{
			continue;
		}

		if (enemy->m_enabled == false)
		{
			continue;
		}
		if (enemy->m_sprite.getPosition().y > 450)
		{
			//INIMIGO VAI PRA BASE 
			enemy->m_sprite.setPosition(100.f, 500.f);

			//ATIVA ARMA SECRETA
			enemy->m_sprite.setTexture(_TextureEnemySecretWeapon);


			//PARA DE MOVIMENTAR OS INIMIGOS
			Stop = true;

			//GAME OVER
			DisplayGameOver();
		}
	}
}


void Game::HandleCollisionWeaponEnemy()
{
	//COLISAO DA BALA DO PLAYER COM O INIMIGO

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemy)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();


			if (boundWeapon.intersects(boundEnemy) == true)
			{
				
				//TEXTURA EXPLOSÃO
				enemy->m_sprite.setTexture(_TextureExplosion);

				//TEM CHANCE DE EXPLODIR E NÃO DESAPARECER
				int k_count = 0;
				int r = rand() % 11; // NUMERO ALEATORIO DE 0 A 19
				if (r != 10)
				{
					k_count++;
					continue;
				}
				if (k_count > 5)
				{
					continue;
				}

				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 10;
				goto end;
			}
		}
		//TIRO NO FLYBY
		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			//CLASSE FLYBY
			if (enemy->m_type != EntityType::flyby)
			{
				continue;
			}
			//VIVO
			if (enemy->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();


			if (boundWeapon.intersects(boundEnemy) == true)
			{

				//TEXTURA EXPLOSÃO
				enemy->m_sprite.setTexture(_TextureExplosion);

				//TEM CHANCE DE EXPLODIR E NÃO DESAPARECER
				int k_count = 0;
				int r = rand() % 13; // NUMERO ALEATORIO DE 0 A 19
				if (r != 10)
				{
					k_count++;
					continue;
				}
				if (k_count > 5)
				{
					continue;
				}
				//MATA FLYBY
				_IsFlybyMoving = false;
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				
				

				
				//SE MORREU EM VOO NAO VOLTA PARA A ESQUADRA
				std::vector<std::shared_ptr<Entity>>::reverse_iterator a = EntityManager::m_Entities.rbegin();
				for (; a != EntityManager::m_Entities.rend(); a++)
				{

					std::shared_ptr<Entity> entity = *a;

					//INIMIGO DESABILITADO 
					if (entity->m_enabled == true)
					{
						continue;
					}
					//QUE VIROU FLYBY
					if (entity->m_flyby == false)
					{
						continue;
					}
					if (entity->m_type != EntityType::enemy)
					{
						continue;
					}
					//MATA ELE
					entity->m_flybydead = true;
					entity->m_flyby = false;
					
				}
				_IsPlayerWeaponFired = false;

				//PONTUAÇÃO
				_score += 15;
				
				//break
				goto end;
			}
		}
	}

end:
	//nop
	return;
}


void Game::HandleCollisionWeaponEnemyMaster()
{
	//COLISAO BALA DO BOSS COM O PLAYER

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemyMaster)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundEnemy) == true)
			{
				
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 100;
				//break;
				goto end;
			}
		}
	}

end:
	//nop
	return;
}

void Game::HandleGameOver()
{
//	ANALISA O GAME OVER / WIN
	int count = std::count_if(EntityManager::m_Entities.begin(), EntityManager::m_Entities.end(), [](std::shared_ptr<Entity> element) {
		if (element->m_type == EntityType::enemy || element->m_type == EntityType::enemyMaster)
		{
			if (element->m_enabled == false)
			{
				return true;
			}
		}
		return false;
	});
	
	// CONDIÇÃO DE VITORIA
	if (count == ((SPRITE_COUNT_X * SPRITE_COUNT_Y) + 1)*count_restart)
	{
		if (Win != 1 && _IsFlybyMoving == false)
		{
			DisplayGameWin();
		}
		
	}
	// GAME OVER
	if (EntityManager::GetPlayer()->m_enabled == false)
	{
		
		DisplayGameOver();
	}
	//GAME OVER
	if (_lives == 0)
	{
		
		DisplayGameOver();
	}

	
}

void Game::DisplayGameOver()
{
	
	if (_lives == 0 || Stop == true)
	{	
		//TROCA TEXTURA DO PLAYER
		EntityManager::GetPlayer()->m_sprite.setTexture(_TexturePlayerExplosion);

		//PAUSA UNIDADES
		Stop = true;

		//PRINTA GAME OVER
		mText.setFillColor(sf::Color::White);
		mText.setFont(mFont);
		mText.setPosition(200.f, 200.f);
		mText.setCharacterSize(80);
		mText.setString("GAME OVER");

	}
	
}

void Game::DisplayGameWin()
{
	//PAUSA O GAME
	Stop = true;
	//PRINTA GAME WIN
		mText.setFillColor(sf::Color::White);
		mText.setFont(mFont);
		mText.setPosition(250.f, 200.f);
		mText.setCharacterSize(80);
		mText.setString("YOU WIN!");
	
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	//RECEBE OS INPUTS DO TECLADO
	if (Stop == false)
	{
		if (key == sf::Keyboard::Left)
			mIsMovingLeft = isPressed;
		if (key == sf::Keyboard::Right)
			mIsMovingRight = isPressed;

		if (key == sf::Keyboard::Space)
		{
			if (isPressed == false)
			{
				return;
			}

			if (_IsPlayerWeaponFired == true)
			{
				return;
			}
			//ATT A POSIÇÃO DO PLAYER
			std::shared_ptr<Entity> sw = std::make_shared<Entity>();
			sw->m_sprite.setTexture(_TextureWeapon);
			sw->m_sprite.setPosition(
				EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2,
				EntityManager::GetPlayer()->m_sprite.getPosition().y - 10);
			sw->m_type = EntityType::weapon;
			sw->m_size = _TextureWeapon.getSize();
			EntityManager::m_Entities.push_back(sw);

			_IsPlayerWeaponFired = true;
		}
	}

	//IMPLEMENTACAO DO PAUSE
	if (key == sf::Keyboard::P)
	{
		if (Pause == false)
		{
			if ((isPressed == true))
			{
				Stop = true;
				Pause = true;
				return;
			}
		}
		if (Pause == true)
		{
			if ((isPressed == true))
			{
				Stop = false;
				Pause = false;
				return;
			}
		}
		
	}

	//IMPLEMENTACAO DO RESTART
	if (key == sf::Keyboard::R)
	{
		
		if ((isPressed == true))
		{
			Win = 1;
			Restart = true;
			return;
		}
	}

	//IMPLEMENTACAO DO ESC

	if (key == sf::Keyboard::Escape)
	{
		if ((isPressed == true))
		{
			mWindow.close();
		}
	}


}
