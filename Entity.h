#pragma once

#include "Components.h"
#include <memory>
#include <string>

class Entity
{
    const size_t        m_id = 0;
    const std::string   m_tag = "Default";
    bool                m_alive = true;

    Entity(const std::string& tag, size_t id);
    friend class EntityManager;

public:
    std::shared_ptr<CCollision>     cCollision;
    std::shared_ptr<CInput>         cInput;
    std::shared_ptr<CLifespan>      cLifespan;
    std::shared_ptr<CLives>         cLives;
    std::shared_ptr<CScore>         cScore;
    std::shared_ptr<CShape>         cShape;
    std::shared_ptr<CTransform>     cTransform;
    std::shared_ptr<CText>          cText;
    
    void destroy();
    const std::string& tag() const;
    bool isActive() const;
    const size_t id() const;
};