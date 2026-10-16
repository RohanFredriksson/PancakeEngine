// Runs the real engine loop to check the inspector's editing actions, spawning prefabs, and hot reloading
// textures and audio from files the test writes and then changes.

#include <cstdio>
#include <cstdint>
#include <fstream>
#include <vector>
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include "test.hpp"
#include "pancake/pancake.hpp"

using namespace Pancake;

namespace {

    const char* TEXTURE_FILE = "test_editor_texture.png";
    const char* AUDIO_FILE = "test_editor_audio.wav";
    const char* PREFAB_FILE = "test_editor_prefab.json";

    int frame = 0;
    int reloadStarted = -1;
    unsigned int textureId = 0;

    void log(const char* message) {
        std::printf("frame %d: %s\n", frame, message);
        std::fflush(stdout);
    }

    void writeImage(int size) {
        std::vector<unsigned char> pixels(size * size * 4, 255);
        stbi_write_png(TEXTURE_FILE, size, size, 4, pixels.data(), size * 4);
    }

    // An 8 bit mono PCM wave of silence.
    void writeWave(uint32_t samples) {
        uint32_t rate = 8000;
        std::ofstream f(AUDIO_FILE, std::ios::binary);
        auto u32 = [&f](uint32_t v) {f.write(reinterpret_cast<const char*>(&v), 4);};
        auto u16 = [&f](uint16_t v) {f.write(reinterpret_cast<const char*>(&v), 2);};
        f.write("RIFF", 4); u32(36 + samples); f.write("WAVE", 4);
        f.write("fmt ", 4); u32(16); u16(1); u16(1); u32(rate); u32(rate); u16(1); u16(8);
        f.write("data", 4); u32(samples);
        std::vector<char> data(samples, (char) 128);
        f.write(data.data(), samples);
    }

    // Moves a file's modified time on, so a change is seen even on file systems with coarse timestamps.
    void touch(const char* filename, int seconds) {
        std::filesystem::last_write_time(filename, std::filesystem::last_write_time(filename) + std::chrono::seconds(seconds));
    }

}

class Driver : public Component {

    public:

        Driver() : Component("Driver") {}

        void update(float dt) override {

            frame++;
            Scene* scene = Pancake::getScene();

            if (frame == 3) {
                log("duplicate, move and delete through the inspector");
                Entity* crate = scene->findEntity("Crate");
                Inspector::select(crate->getId());
                int copyId = Inspector::duplicateSelected();
                CHECK(copyId >= 0 && copyId != crate->getId());
                CHECK(Inspector::getSelected() == copyId);
                Entity* copy = scene->getEntity(copyId);
                CHECK(copy != nullptr && copy->hasComponent<Rigidbody>());
                CHECK(copy->getPosition() == crate->getPosition() + vec2(0.5f, 0.5f));
                CHECK(scene->findEntities("Crate").size() == 2);

                copy->getComponent<Rigidbody>()->setVelocity(5.0f, 5.0f);
                Inspector::moveSelected(vec2(3.0f, 0.0f));
                CHECK(copy->getPosition().x == crate->getPosition().x + 3.5f);
                CHECK(copy->getComponent<Rigidbody>()->getVelocity() == vec2(0.0f)); // Stopped while held.

                CHECK(Inspector::deleteSelected());
                CHECK(copy->isDead());
                CHECK(Inspector::getSelected() == -1);
                CHECK(!Inspector::deleteSelected());
                CHECK(Inspector::duplicateSelected() == -1);
            }

            if (frame == 5) {
                log("save a prefab and spawn it from code and the console");
                Console::execute("saveprefab Crate " + std::string(PREFAB_FILE));
                Entity* a = Pancake::spawn(PREFAB_FILE, vec2(-4.0f, 3.0f));
                CHECK(a != nullptr && a->getPosition() == vec2(-4.0f, 3.0f));
                Console::execute("prefab " + std::string(PREFAB_FILE) + " 4 3");
                Console::execute("clone Crate 0 6");
                CHECK(scene->findEntities("Crate").size() == 4);
                CHECK(Pancake::spawn("no_such_prefab.json", vec2(0.0f)) == nullptr);
            }

            if (frame == 6) {
                log("load a texture and a sound from files, then change the files");
                writeImage(2);
                writeWave(800);
                Texture* texture = TexturePool::get(TEXTURE_FILE);
                CHECK(!texture->isMissing() && texture->getWidth() == 2);
                textureId = texture->getId();
                CHECK_NEAR(AudioPool::get(AUDIO_FILE)->getLength(), 0.1, 1e-3);

                Pancake::hotReload(true);
                writeImage(4);
                writeWave(1600);
                touch(TEXTURE_FILE, 2);
                touch(AUDIO_FILE, 2);
                reloadStarted = frame;
            }

            // Hot reload checks twice a second of real time, so wait for it.
            if (reloadStarted > 0 && TexturePool::get(TEXTURE_FILE)->getWidth() == 4) {
                log("the texture and sound were reloaded in place");
                Texture* texture = TexturePool::get(TEXTURE_FILE);
                CHECK(texture->getId() == textureId);
                CHECK(texture->getHeight() == 4);
                CHECK_NEAR(AudioPool::get(AUDIO_FILE)->getLength(), 0.2, 1e-3);
                CHECK(AssetPool::reloadChanged() == 0); // Nothing has changed since.

                // Reloading on request, with hot reload off.
                Pancake::hotReload(false);
                writeImage(8);
                touch(TEXTURE_FILE, 4);
                Console::execute("reload");
                CHECK(texture->getWidth() == 8);
                reloadStarted = -1;
                Pancake::stop();
            }

            if (reloadStarted > 0 && frame - reloadStarted > 5000) {log("hot reload never happened"); CHECK(false); Pancake::stop();}

        }

};

void init(Scene* scene) {
    Entity* crate = new Entity(vec2(0.0f, 0.0f));
    crate->setName("Crate");
    crate->addComponent<SpriteRenderer>();
    BoxCollider* box = new BoxCollider();
    box->setMass(1.0f);
    crate->addComponent<Rigidbody>()->addCollider(box)->setGravityScale(0.0f);
    scene->addEntity(crate);

    Entity* d = new Entity();
    d->addComponent(new Driver());
    scene->addEntity(d);
}

int main() {
    Pancake::load(init);
    Pancake::start();
    CHECK(reloadStarted == -1 && frame > 6);
    std::remove(TEXTURE_FILE);
    std::remove(AUDIO_FILE);
    std::remove(PREFAB_FILE);
    std::printf("engine ran %d frames, %d failed checks\n", frame, PancakeTest::failures());
    return PancakeTest::failures() == 0 ? 0 : 1;
}
