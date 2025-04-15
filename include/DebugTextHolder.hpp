#ifndef DEBUG_TEXT_HOLDER_HPP
#define DEBUG_TEXT_HOLDER_HPP

#include "Text.hpp"
#include <vector>
namespace debug
{
    class DebugTextHolder
    {
    public:
        static DebugTextHolder& instance();
        unsigned int addText(const std::string& text);
        void changeText(unsigned int index, const std::string& text);
        void update(float deltaTime);
        void clear();
        void removeText(unsigned int index);
        void setScale(float scale);

    private:
        DebugTextHolder() = default;
        DebugTextHolder(const DebugTextHolder&) = delete;
        DebugTextHolder& operator=(const DebugTextHolder&) = delete;

        float m_margin{50.0f};

        std::vector<Text> m_texts;
    };
}

#endif //DEBUG_TEXT_HOLDER_HPP
