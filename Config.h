#ifndef CONFIG_H
#define CONFIG_H

class Config {
public:
    static constexpr int TILE_SIZE = 30;
    static constexpr int FPS = 10;
    static constexpr int RENDER_START_OFFSET = 9;
    static constexpr int RENDER_END_OFFSET = 10;
    static constexpr float WALL_LIMIT = 0.4;
    static constexpr int ENEMY_NB = 80;
    static constexpr int POTION_NB = 80;

};

#endif // CONFIG_H
