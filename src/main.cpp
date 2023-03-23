#include "pancake/core/engine.hpp"
#include "pancake/scenes/title.hpp"

int main() {
    return Engine::run("Title", TitleInit);
}