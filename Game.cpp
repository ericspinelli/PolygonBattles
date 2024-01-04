#include "Game.h"
#include "Entity.h"
#include "EntityManager.h"
#include <cmath>

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& config)
{
    //Read in config file
    std::ifstream fin(config);
    std::string temp;

    while (fin >> temp)
    {
        // Clear comment lines from config
        if (temp == "//") {std::getline(fin, temp);}

        if (temp == "Window")
        {
            int W, H, FL, FS;                               // Width, Height, FrameLimit, FullScreen
            fin >> W >> H >> FL >> FS;
            m_window.create(sf::VideoMode(W, H), "Polygon Battles!", FS);
            m_window.setFramerateLimit(FL);
        }
        if (temp == "Player")
        {
            fin >> m_playerConfig.L                                             // Lives
                >> m_playerConfig.I                                             // Invulnerable (frames)
                >> m_playerConfig.SR                                            // ShapeRadius
                >> m_playerConfig.CR                                            // CollisionRadius
                >> m_playerConfig.S                                             // Speed
                >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB  // FillColor(R,G,B)
                >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB  // OutlineColor(R,G,B)
                >> m_playerConfig.IR >> m_playerConfig.IG >> m_playerConfig.IB  // InvulnerableOutlineColor(R,G,B)
                >> m_playerConfig.OT                                            // OutlineThickness
                >> m_playerConfig.V;                                            // ShapeVertices
        }
        if (temp == "Enemy")
        {       
            fin >> m_enemyConfig.SR                                             // ShapeRadius
                >> m_enemyConfig.CR                                             // CollisionRadius
                >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX                     // Speed(Min,Max)
                >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB     // OutlineColor(R,G,B)
                >> m_enemyConfig.OT                                             // OutlineThickness
                >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX                     // Vertices(Min,Max)
                >> m_enemyConfig.L                                              // SmallLifespan
                >> m_enemyConfig.SD                                             // SmallSpeedDivision
                >> m_enemyConfig.SI;                                            // SpawnInterval
        }
        if (temp == "Bullet")
        {      
            fin >> m_bulletConfig.SR                                            // ShapeRadius
                >> m_bulletConfig.CR                                            // CollisionRadius
                >> m_bulletConfig.S                                             // Speed
                >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB  // FillColor(R,G,B)
                >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB  // OutlineColor(R,G,B)
                >> m_bulletConfig.OT                                            // OutlineThickness
                >> m_bulletConfig.V                                             // ShapeVertices
                >> m_bulletConfig.L;                                            // Lifespan
        }
        if (temp == "Font")
        {
            fin >> m_fontConfig.FF                                              // FontFile
                >> m_fontConfig.FS                                              // FontSize
                >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;          // FontColor(R, G, B)
        }
    }

    // Load game font
    m_font.loadFromFile(m_fontConfig.FF);
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnScoreText()
{
    // Create a new score via EntityManager
    auto entity = m_entities.addEntity("score");

    std::string score = "SCORE 0";
    entity->cText = std::make_shared<CText>(score, m_font, 
                                            Vec2(30.0f, 20.0f),
                                            m_fontConfig.FS,
                                            sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
    m_scoreText = entity;
}

void Game::spawnLivesText()
{
    // Create a new score via EntityManager
    auto entity = m_entities.addEntity("lives");

    std::string lives = "LIVES ";
    lives += std::to_string(m_player->cLives->remaining);
    entity->cText = std::make_shared<CText>(lives, m_font, 
                                            Vec2(m_window.getSize().x - 150, 20.0f),
                                            m_fontConfig.FS,
                                            sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
    m_livesText = entity;
}

void Game::spawnLevelText()
{
    // Create a new score via EntityManager
    auto entity = m_entities.addEntity("level");

    std::string level = "LEVEL ";
    level += std::to_string(m_level);
    entity->cText = std::make_shared<CText>(level, m_font, 
                                            Vec2(m_window.getSize().x / 2.0f - 40, 20.0f),
                                            m_fontConfig.FS,
                                            sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
    m_levelText = entity;
}

void Game::spawnPlayer()
{
    // Create a new player via EntityManager
    auto entity = m_entities.addEntity("player");

    // Calculate spawning position (middle of window)
    float center_x = m_window.getSize().x / 2.0f;
    float center_y = m_window.getSize().y / 2.0f;
    
    // Add player components
    entity->cTransform = std::make_shared<CTransform>(Vec2(center_x, center_y), Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);
    entity->cShape = std::make_shared<CShape>   (m_playerConfig.SR, m_playerConfig.V, 
                                                sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
                                                sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), 
                                                m_playerConfig.OT);
    entity->cInput = std::make_shared<CInput>();
    entity->cCollision = std::make_shared<CCollision>(32.0f);
    entity->cScore = std::make_shared<CScore>(0);
    entity->cLives = std::make_shared<CLives>(m_playerConfig.L, m_playerConfig.I);

    // Set player entity to easily accessible member variable
    m_player = entity;
}

void Game::spawnEnemy()
{
    if (m_entities.getEntities("player").size() > 0)  // Stop spawning enemies if player is out of lives
    {
        // Create a new enemy via EntityManager
        auto entity = m_entities.addEntity("enemy");

        // Randomize enemy parameters using ranges from config
        srand(time(NULL) + m_spawnQueue);

        float enemy_x;
        float enemy_y;
        Vec2 enemy_pos;
        do  // ensure enemy doesn't spawn on player
        {
            enemy_pos.x = (rand() % (m_window.getSize().x - (2 * m_enemyConfig.SR))) + m_enemyConfig.SR;
            enemy_pos.y = (rand() % (m_window.getSize().y - (2 * m_enemyConfig.SR))) + m_enemyConfig.SR;
            
        } while ( m_player->cTransform->pos.dist(enemy_pos) <= (2 * (m_enemyConfig.CR + m_player->cCollision->radius)));
        // b->cTransform->pos.dist(e->cTransform->pos) <= (e->cCollision->radius + b->cCollision->radius

        int speed_range = m_enemyConfig.SMAX - m_enemyConfig.SMIN;
        float enemy_speed = (rand() % (speed_range + 1)) + m_enemyConfig.SMIN;
        float enemy_angle = (rand() % 360);
        int enemy_r = (rand() % 256);
        int enemy_g = (rand() % 256);
        int enemy_b = (rand() % 256);
        int enemy_vertices = (rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1)) + m_enemyConfig.VMIN; 

        // Add enemy components
        entity->cTransform = std::make_shared<CTransform>(  enemy_pos, Vec2(enemy_speed*cosf(enemy_angle), enemy_speed*sinf(enemy_angle)), enemy_angle);
        entity->cShape = std::make_shared<CShape>        (  m_enemyConfig.SR, enemy_vertices, 
                                                            sf::Color(enemy_r, enemy_g, enemy_b),
                                                            sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), 
                                                            m_enemyConfig.OT);
        entity->cCollision = std::make_shared<CCollision>(  m_enemyConfig.CR);
        entity->cScore = std::make_shared<CScore>        (  enemy_vertices * 100);
    }
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> parent)
{
    // Get number of small enemies to spawn
    int num = parent->cShape->circle.getPointCount();

    // Calculate spawn direction basics
    float angle_interval = (atan2(1, 1)*8)/num; // (atan(1) * 4 = PI) -> (2*PI / num)
    
    float current_angle = atan2(parent->cTransform->velocity.y, parent->cTransform->velocity.x);
    float speed_length = parent->cTransform->velocity.length()/m_enemyConfig.SD;

    for (int i = 0; i < num; i ++)
    {
        // Calculate spawn direction specifics, then increment angle
        Vec2 small_vel = Vec2(speed_length*cosf(current_angle), speed_length*sinf(current_angle));

        // Create a new enemy via EntityManager
        auto small = m_entities.addEntity("enemy");

        // Add enemy components (half size of parent, half speed/same direction, same shape and color, double score, add lifetime)
        small->cTransform = std::make_shared<CTransform>(   parent->cTransform->pos, small_vel, current_angle);
        small->cShape = std::make_shared<CShape>        (   parent->cShape->circle.getRadius()/2, parent->cShape->circle.getPointCount(),
                                                            parent->cShape->circle.getFillColor(),
                                                            sf::Color(255, 255, 255),
                                                            parent->cShape->circle.getOutlineThickness());
        small->cCollision = std::make_shared<CCollision>(   parent->cCollision->radius/2);
        small->cScore = std::make_shared<CScore>        (   parent->cScore->score * 2);
        small->cLifespan = std::make_shared<CLifespan>  (   m_enemyConfig.L);

        // Increment angle for next small enemy
        current_angle += angle_interval;
    }


}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
    // Only fire new bullet if past bullet cooldown time
    if ((m_currentFrame - m_lastBulletTime) > m_bulletCooldown) 
    {
        m_lastBulletTime = m_currentFrame;
        
        // Create new bullet
        auto e = m_entities.addEntity("bullet");

        // Calculate angle from player to mouse position
        float bullet_angle = atan2f((mousePos.y - entity->cTransform->pos.y), (mousePos.x - entity->cTransform->pos.x));

        // Add bullet components
        e->cTransform = std::make_shared<CTransform>(   Vec2(entity->cTransform->pos.x /*+ entity->cCollision->radius*/, entity->cTransform->pos.y /*+ entity->cCollision->radius*/),
                                                        Vec2(m_bulletConfig.S*cosf(bullet_angle), m_bulletConfig.S*sinf(bullet_angle)), bullet_angle);
        e->cShape = std::make_shared<CShape>        (   m_bulletConfig.SR, m_bulletConfig.V,
                                                        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
                                                        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
                                                        m_bulletConfig.OT);
        e->cCollision = std::make_shared<CCollision>(   m_bulletConfig.CR);
        e->cLifespan = std::make_shared<CLifespan>  (   m_bulletConfig.L);
    }
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
    // Only fire special weapon if past bullet cooldown time
    if ((m_currentFrame - m_lastSpecialWeaponTime) > m_specialWeaponCooldown) 
    {
        m_lastSpecialWeaponTime = m_currentFrame;
        
        // Calculate angle from player to mouse position
        int num = 30;
        //float bullet_angle = atan2f((mousePos.y - entity->cTransform->pos.y), (mousePos.x - entity->cTransform->pos.x));
        float bullet_angle = 0.0f;
        float angle_interval = (atan2(1, 1)*8) / num; // (atan(1) * 4 = PI) -> (2*PI / num)

        // Create num bullets
        for (int i = 0; i < num; i++)
        {
            auto e = m_entities.addEntity("bullet");
            bullet_angle += angle_interval;

            // Add bullet components
            e->cTransform = std::make_shared<CTransform>(   Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y),
                                                            Vec2(m_bulletConfig.S*cosf(bullet_angle), m_bulletConfig.S*sinf(bullet_angle)), bullet_angle);
            e->cShape = std::make_shared<CShape>        (   m_bulletConfig.SR, m_bulletConfig.V,
                                                            sf::Color::Red,
                                                            sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
                                                            m_bulletConfig.OT);
            e->cCollision = std::make_shared<CCollision>(   m_bulletConfig.CR);
            e->cLifespan = std::make_shared<CLifespan>  (   m_bulletConfig.L);
        }
    }
}

void Game::spawnText(const std::string& text, Vec2& pos, int size, sf::Color& color)
{
    // Create a new textbox via EntityManager
    auto entity = m_entities.addEntity("text"); 

    // Add textbox components
    entity->cText = std::make_shared<CText>(text, m_font, pos, size, color);
}

void Game::sEnemySpawner()
{
    // Only spawn new enemy if past enemy cooldown time
    
    /*if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SI)
    {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }*/

    if (m_spawnQueue > 0)
    {
        spawnEnemy();
        m_spawnQueue--;
    }
    else if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SI)
    {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
        m_spawnQueue = m_spawnQueueMax;
    }
}

void Game::sMovement() 
{
    // Enemy movement
    for (auto e : m_entities.getEntities("enemy"))
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;

        // Check if there is a Lifespan (small enemies only) and fade out
        if (e->cLifespan)
        {
            // Only move small enemy if it has Lifespan remaining
            if (e->cLifespan->remaining)
            {
                e->cTransform->pos.x += e->cTransform->velocity.x;
                e->cTransform->pos.y += e->cTransform->velocity.y;
                e->cLifespan->remaining--;

                // Fade out small enemy as it travels
                sf::Color current = e->cShape->circle.getFillColor();
                float alpha = (float)e->cLifespan->remaining / (float)e->cLifespan->total;
                current.a = (int)(alpha * 255);
                e->cShape->circle.setFillColor(current);
                e->cShape->circle.setOutlineColor(current);
            }
            // Destroy bullet when lifetime is over
            else
            {
                e->destroy();
            }        
        }
    }

    // Player movement
    for (auto p : m_entities.getEntities("player"))
    {
        // Wall detection
        bool top = ((p->cTransform->pos.y - p->cCollision->radius) < 0);
        bool bottom = ((p->cTransform->pos.y + p->cCollision->radius) > m_window.getSize().y);
        bool left = ((p->cTransform->pos.x - p->cCollision->radius) < 0);
        bool right = ((p->cTransform->pos.x + p->cCollision->radius) > m_window.getSize().x);

        // Position + Velocity
        if (p->cInput->up && !top) {p->cTransform->pos.y -= p->cTransform->velocity.y;}
        if (p->cInput->down && !bottom) {p->cTransform->pos.y += p->cTransform->velocity.y;}
        if (p->cInput->left && !left) {p->cTransform->pos.x -= p->cTransform->velocity.x;}
        if (p->cInput->right && !right) {p->cTransform->pos.x += p->cTransform->velocity.x;}
    }

    // Bullet movement
    for (auto b : m_entities.getEntities("bullet"))
    {
        // Only move bullet if it has Lifespan remaining
        if (b->cLifespan->remaining)
        {
            b->cTransform->pos.x += b->cTransform->velocity.x;
            b->cTransform->pos.y += b->cTransform->velocity.y;
            b->cLifespan->remaining--;

            // Fade out bullet as it travels
            sf::Color current = b->cShape->circle.getFillColor();
            float alpha = (float)b->cLifespan->remaining / (float)b->cLifespan->total;
            current.a = (int)(alpha * 255);
            b->cShape->circle.setFillColor(current);
            b->cShape->circle.setOutlineColor(current);
        }
        // Destroy bullet when lifetime is over
        else
        {
            b->destroy();
        }
    }
}

void Game::sWeapons()
{
    if (m_player->cInput->shoot)
    {
        Vec2 mouse_pos = Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y);
        spawnBullet(m_player, mouse_pos);
        m_player->cInput->shoot = false;
    }
    if (m_player->cInput->special)
    {
        Vec2 mouse_pos = Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y);
        spawnSpecialWeapon(m_player, mouse_pos);
        m_player->cInput->special = false;
    }
}

void Game::sCollision()
{
    for (auto e : m_entities.getEntities("enemy"))
    {
        // Enemy wall collision
        if (    ((e->cTransform->pos.x - e->cCollision->radius) < 0) ||
                ((e->cTransform->pos.x + e->cCollision->radius) > m_window.getSize().x))
            { e->cTransform->velocity.x *= -1; }
        if (    ((e->cTransform->pos.y - e->cCollision->radius) < 0) ||
                ((e->cTransform->pos.y + e->cCollision->radius) > m_window.getSize().y))
            { e->cTransform->velocity.y *= -1; }

        // Bullet-enemy collision
        for (auto b : m_entities.getEntities("bullet"))
        {            
            // Collision if distance between bullet and enemy is less than sum of collision radii
            if (b->cTransform->pos.dist(e->cTransform->pos) <= (e->cCollision->radius + b->cCollision->radius))
            {
                m_player->cScore->score += e->cScore->score;
                std::string score = "SCORE ";
                score += std::to_string(m_player->cScore->score);
                m_scoreText->cText->text.setString(score);
                
                b->destroy();
                e->destroy();
                
                // If no Lifespan (not small enemy), spawn small enemy
                if (!e->cLifespan)
                {
                    spawnSmallEnemies(e);
                }
            }
        }

        // Player-enemy collision
        for (auto p : m_entities.getEntities("player"))
        {
            if (p->cTransform->pos.dist(e->cTransform->pos) <= (e->cCollision->radius + p->cCollision->radius) && (!p->cLives->invulnerable))
            {
                p->cLives->remaining--;
                std::string lives = "LIVES ";
                lives += std::to_string(p->cLives->remaining);
                m_livesText->cText->text.setString(lives);

                e->destroy();
                
                if (p->cLives->remaining > 0)
                {
                    // Calculate spawning position (middle of window)
                    float center_x = m_window.getSize().x / 2.0f;
                    float center_y = m_window.getSize().y / 2.0f;

                    p->cTransform->pos.x = center_x;
                    p->cTransform->pos.y = center_y;

                    // Make player temporarily invulnerable
                    p->cLives->invulnerable = p->cLives->invulnMax;
                }
                else
                {
                    p->destroy();
                    
                    // Display GAME OVER in the middle of the screen
                    std::string text = "GAME OVER";
                    Vec2 pos = Vec2((m_window.getSize().x)/2.0f - 135, (m_window.getSize().y)/2.0f - 30);
                    sf::Color color(255, 0, 0);
                    spawnText(text, pos, 60, color);
                }
            }
        }
    }

    // Player wall collision is done in Game::sMovement()
}

void Game::sInvulnerability()
{
    if (m_player->cLives->invulnerable > 0)
    {
        // Set player outline to indicate invulnerability
        m_player->cShape->circle.setOutlineColor(sf::Color(m_playerConfig.IR, m_playerConfig.IG, m_playerConfig.IB));

        m_player->cLives->invulnerable--;
    }
    else
    {
        // Reset player outline to indicate vulnerability (normal game mode)
        m_player->cShape->circle.setOutlineColor(sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB));
    }
}

void Game::sIncreaseDifficulty()
{
    // Increase level based on player score
    if (m_player->cScore->score >= m_nextLevel)
    {
        // Increase level and points needed for next level
        m_level++;
        m_nextLevel += m_level * 10000;

        // Update textbox in window
        std::string level = "LEVEL ";
        level += std::to_string(m_level);
        m_levelText->cText->text.setString(level);

        // ODD LEVELS: Increase spawn queue max
        if ((m_level > 1) && (m_level % 2 == 1))
        {
            std::cout << "odd" << std::endl;
            m_spawnQueueMax += 1;
        }
        
        // EVEN LEVELS: Increases SpeedMix and SpeedMax (faster movement)
        if ((m_level % 2 == 0))
        {
            std::cout << "even" << std::endl;
            m_enemyConfig.SMIN += 1;
            m_enemyConfig.SMAX += 1;
        }
    }
}

void Game::sUserInput()
{
    // sUserInput only checks inputs and sets flags. Movement, weapons, etc are handled by separate systems

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed) { m_running = false; }
    
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::P:
                m_paused = !m_paused;
                break;
            case sf::Keyboard::Escape:
                m_running = false;
            }
        }

        if ((!m_paused) && (m_player->cLives->remaining > 0)) // when NOT paused AND player is alive, allow all input
        {
            // this event triggers when a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true; 
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true; 
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true; 
                    break;
                }
            }

            // this event triggers when a key is released
            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                    break;
                }
            }

            // mouse for shooting weapons
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                m_player->cInput->shoot = true;
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                m_player->cInput->special = true;
            }
        }
    }
}

void Game::renderMoveable(EntityVec& vec)
{
    for (auto e : vec)
    {
        // set position based on cTransform->pos
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    
        // set rotation based on cTransform->angle
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        
        // draw the entity's sf::CircleShape
        m_window.draw(e->cShape->circle);
    }
}

void Game::renderText(EntityVec& vec)
{
    for (auto e : vec)
    {       
        // draw the entity's sf::Text
        m_window.draw(e->cText->text);
    }
}

void Game::sRender()
{
    m_window.clear();

    renderMoveable(m_entities.getEntities("player"));
    renderMoveable(m_entities.getEntities("enemy"));
    renderMoveable(m_entities.getEntities("bullet"));

    renderText(m_entities.getEntities("text"));
    renderText(m_entities.getEntities("score"));
    renderText(m_entities.getEntities("lives"));
    renderText(m_entities.getEntities("level"));
    
    m_window.display();
}

void Game::run()
{
    spawnPlayer();
    spawnScoreText();
    spawnLivesText();
    spawnLevelText();

    while (m_running)
    {
        // Add and remove entities as a delayed effect to avoid iterator invalidation
        m_entities.update();

        // Disable spawning, movement, collisions, and difficulty increase when paused
        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            sWeapons();
            sCollision();
            sInvulnerability();
            sIncreaseDifficulty();
        }

        // Enable input and rendering even when paused
        sUserInput();
        sRender();

        m_currentFrame++;
    }
}