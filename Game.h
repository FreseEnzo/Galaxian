#pragma once
#include "Weapon.h"

#define SPRITE_COUNT_X 11
#define SPRITE_COUNT_Y 5
#define BLOCK_COUNT 4

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();

	void InitSprites();
	void ResetSprites();
	void RestartGame();

	void updateStatistics(sf::Time elapsedTime);
	void HandleTexts();
	void HandleCollisionEnemyMasterWeaponPlayer();
	void HandleEnemyMasterWeaponMoves();
	void HandleEnemyMasterWeaponFiring();
	
	void HandleEnemyMasterMove();

	void HandleCollisionWeaponPlayer();
	void HandleEnemyWeaponMoves();
	void HandleEnemyWeaponFiring();

	void HandleEnemyMoves();
	void HandleWeaponMoves();
	void HandleCollisionWeaponEnemy();
	void HandleCollisionWeaponEnemyMaster();
	void HandleGameOver();
	void DisplayGameOver();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void ReachesGround();
	void Flyby();
	void HandleFlybyMoves();
	void HandleFlybyWeaponMoves();
	void HandleFlybyWeaponFiring();
	void DisplayGameWin();
	
	
private:
	static const float		PlayerSpeed;
	static const sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Font	mFont;
	sf::Text	mStatisticsText;
	sf::Time	mStatisticsUpdateTime;
	sf::Time    mTime;
	sf::Text	mText;
	sf::Text	_LivesText;
	int _lives = 3;
	sf::Text	_ScoreText;
	
	sf::Text	_DebugText;
	int _score = 0;
	int count_exploded;
	int count_restart = 1;

	std::size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;
	
	
	bool Stop = false;
	bool Pause = false;
	bool Restart = false;
	int Win = 0;

	bool  secondfly = false;
	bool _IsFlybyMoving = false;
	bool _IsGameOver = false;
	bool _IsEnemyWeaponFired = false;
	bool _IsPlayerWeaponFired = false;
	bool _IsEnemyMasterWeaponFired = false;
	bool _IsFlybyWeaponFired = false;
	
	sf::Texture	_TextureEnemy;
	sf::Sprite	_Enemy[SPRITE_COUNT_X][SPRITE_COUNT_Y];
	sf::Texture	_TextureBlock;
	sf::Sprite	_Block[BLOCK_COUNT];
	sf::Texture	_TextureWeapon;
	sf::Texture	_TextureWeaponEnemy;
	sf::Texture	_TextureWeaponEnemyMaster;
	sf::Sprite	_Weapon;
	sf::Texture	_TextureEnemyMaster;
	sf::Sprite	_EnemyMaster;
	sf::Texture _TextureExplosion;
	sf::Texture _TextureEnemySecretWeapon; 
	sf::Texture	_TextureEnemyMasterExplosion;
	sf::Texture	_TexturePlayerExplosion;
	sf::Texture	_TextureWeaponFlyby;
};

