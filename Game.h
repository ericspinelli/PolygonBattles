#include "Entity.h"
#include "EntityManager.h"
#include <fstream>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>

struct PlayerConfig { int L, I, SR, CR, FR, FG, FB, OR, OG, OB, IR, IG, IB, OT, V; float S; };
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SD, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
struct FontConfig   { int FS, R, G, B; std::string FF; };

class Game
{
    sf::RenderWindow    m_window;
    sf::Font            m_font;
    sf::Text            m_text;

    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    FontConfig          m_fontConfig;

    EntityManager       m_entities;
    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_scoreText;
    std::shared_ptr<Entity> m_livesText;
    std::shared_ptr<Entity> m_levelText;
    
    int                 m_level = 1;
    int                 m_nextLevel = 10000;
    int                 m_spawnQueue = 0;
    int                 m_spawnQueueMax = 0;

    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    int                 m_lastBulletTime = 0;
    int                 m_bulletCooldown = 7;
    int                 m_lastSpecialWeaponTime = 0;
    int                 m_specialWeaponCooldown = 120;

    bool                m_paused = false;
    bool                m_running = true;

    // Initialization & spawning
    void init(const std::string& config);
    void setPaused(bool paused);
    
    void spawnScoreText();
    void spawnLivesText();
    void spawnLevelText();
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> parent);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2& mousePos);
    void spawnText(const std::string& text, Vec2& pos, int size, sf::Color& color);

    // Update manager (delayed effects, etc)
    void update();

    // Systems
    void sEnemySpawner();
    void sMovement();
    void sWeapons();
    void sCollision();
    void sInvulnerability();
    void sIncreaseDifficulty();
    void sUserInput();
    void sRender();
        void renderMoveable (EntityVec& vec);
        void renderText     (EntityVec& vec);


public:
    Game(const std::string& config);

    void run();
};