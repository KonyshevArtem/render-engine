#include "renderer.h"

Renderer::Renderer(shared_ptr<GameObject> _gameObject) :
    m_GameObject(std::move(_gameObject))
{
}
