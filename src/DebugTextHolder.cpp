#include "DebugTextHolder.hpp"

#include "Filesystem.hpp"
#include "WindowsManager.hpp"

debug::DebugTextHolder& debug::DebugTextHolder::instance()
{
    static debug::DebugTextHolder instance;
    return instance;
}

unsigned int debug::DebugTextHolder::addText(const std::string &text)
{
    Text debugText;
    const auto shadersDirectory = filesystem::getShadersFolderPath();
    const std::string font_name = filesystem::getFontsFolderPath().string() + "/ghostmane_font.ttf";

    debugText.initShader(shadersDirectory.string() + "/text.vert", shadersDirectory.string() + "/text.frag");
    debugText.setText(text);
    debugText.setFont(font_name);
    debugText.setScale(0.5f);

    glm::vec2 position;

    position.x = 0.0f;

    position.y = (!m_texts.empty() ? m_texts.back().getY() : window::WindowsManager::instance().getCurrentWindow()->getHeight()) - m_margin;

    debugText.setPosition(position.x, position.y);

    m_texts.push_back(debugText);

    return m_texts.size() - 1;
}

void debug::DebugTextHolder::changeText(const unsigned int index, const std::string &text)
{
    if (index >= m_texts.size())
        return;

    auto& debugText = m_texts[index];

    debugText.setText(text);
}

void debug::DebugTextHolder::update(float deltaTime)
{
    for (auto& text : m_texts)
        text.draw();
}

void debug::DebugTextHolder::clear()
{
    m_texts.clear();
}

void debug::DebugTextHolder::removeText(const unsigned int index)
{
    if (index >= m_texts.size())
        return;

    m_texts.erase(m_texts.begin() + index);
}

void debug::DebugTextHolder::setScale(float scale)
{
    for (auto& text : m_texts)
        text.setScale(scale);
}

