#include "Entity.h"

Entity::Entity(const std::string& tag, size_t id)
        : m_tag(tag)
        , m_id(id)
    {}

void Entity::destroy() {m_alive = false;}
const std::string& Entity::tag() const {return m_tag;}
bool Entity::isActive() const {return m_alive;}
const size_t Entity::id() const {return m_id;}

/*
//Entity Class
class Entity
{
    const size_t        m_id = 0;
    const std::string   m_tag = "Default";
    bool                m_alive = true;

    Entity(const std::string& tag, size_t id)
        : m_tag(tag)
        , m_id(id)
    {}
    friend class EntityManager;

public:
    std::shared_ptr<CCollision>     cCollision;
    std::shared_ptr<CInput>         cInput;
    std::shared_ptr<CLifespan>      cLifespan;
    std::shared_ptr<CScore>         cScore;
    std::shared_ptr<CShape>         cShape;
    std::shared_ptr<CTransform>     cTransform;

    void destroy() {m_alive = false;}
    const std::string& tag() {return m_tag;}
    bool isActive() const {};
    const size_t id() const {return m_id;}
};*/