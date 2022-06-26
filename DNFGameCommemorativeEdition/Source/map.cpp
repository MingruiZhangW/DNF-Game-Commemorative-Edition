#include "map.hpp"
#include "constant.hpp"
#include "floor.hpp"
#include "background.hpp"

// clang-format off

// Scene one floor tiles
// 0 for glass, 1 for road
// First vector and second vector represents matrix
// Third vector represents tile map blend at location (arts with alpha channel)
const std::vector<std::vector<std::vector<int>>> scene_one_floor_tiles = {
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}}
};

// Scene one background tiles
// 0 for far, 1 for mid
const std::vector<std::vector<std::vector<int>>> scene_one_background_tiles = {
    {{0, 1}, {0, 1}}
};

// clang-format on

Map::Map(ShaderProgram* shader, GLfloat width, GLfloat height)
    : SceneNode(StringContant::sceneOneMapName)
    , m_shader(shader)
    , m_window_width(width)
    , m_window_height(height)
{}

Map::~Map() {}

void
Map::initSceneOneMap()
{
    // Background tile
    float rowNum = static_cast<float>(scene_one_background_tiles.size());
    float colNum = static_cast<float>(scene_one_background_tiles[0].size());

    for (size_t i = 0; i < rowNum; i++) {
        for (size_t j = 0; j < colNum; j++) {
            auto heightScale = (m_window_height * RatioContant::backgroundHeightScaleRatio)
                               / rowNum;
            auto upTranslation = (m_window_height
                                  * (1.0f - RatioContant::backgroundHeightScaleRatio))
                                 / rowNum;

            for (size_t k = 0; k < scene_one_background_tiles[i][j].size(); k++) {
                if (scene_one_background_tiles[i][j][k] == 0) {
                    Background* farBack = new Background(StringContant::sceneOneBackgroundName
                                                             + std::to_string(i + j + k),
                                                         m_shader,
                                                         0,
                                                         heightScale,
                                                         Background::BackgroundTextureType::Far);

                    farBack->translate(
                        glm::vec3(farBack->getTextureGeo().x * j, upTranslation * (i + 1), 0.0f));

                    addChild(farBack);
                } else if (scene_one_background_tiles[i][j][k] == 1) {
                    Background* midBack = new Background(StringContant::sceneOneBackgroundName
                                                             + std::to_string(i + j + k),
                                                         m_shader,
                                                         0,
                                                         heightScale,
                                                         Background::BackgroundTextureType::Mid);

                    midBack->translate(
                        glm::vec3(midBack->getTextureGeo().x * j, upTranslation * (i + 1), 0.0f));

                    addChild(midBack);
                }
            }
        }
    }

    // Floor tile
    rowNum = static_cast<float>(scene_one_floor_tiles.size());
    colNum = static_cast<float>(scene_one_floor_tiles[0].size());

    for (size_t i = 0; i < rowNum; i++) {
        for (size_t j = 0; j < colNum; j++) {
            auto heightScale = (m_window_height * RatioContant::floorHeightScaleRatio) / rowNum;

            for (size_t k = 0; k < scene_one_floor_tiles[i][j].size(); k++) {
                if (scene_one_floor_tiles[i][j][k] == 0) {
                    Floor* glassFloor = new Floor(StringContant::sceneOneFloorName
                                                      + std::to_string(i + j + k),
                                                  m_shader,
                                                  0,
                                                  heightScale,
                                                  Floor::FloorTextureType::Grass);

                    if (i != rowNum - 1) {
                        glassFloor->scale(glm::vec3(1.0f, RatioContant::attachOffset, 1.0f));
                    }
                    glassFloor->translate(
                        glm::vec3(glassFloor->getTextureGeo().x * j, heightScale * i, 0.0f));

                    addChild(glassFloor);
                } else if (scene_one_floor_tiles[i][j][k] == 1) {
                    Floor* roadFloor = new Floor(StringContant::sceneOneFloorName
                                                     + std::to_string(i + j + k),
                                                 m_shader,
                                                 0,
                                                 heightScale,
                                                 Floor::FloorTextureType::Road);

                    roadFloor->translate(
                        glm::vec3(roadFloor->getTextureGeo().x * j, heightScale * i, 0.0f));

                    addChild(roadFloor);
                }
            }
        }
    }
}

void
Map::updateWindowGeo()
{}