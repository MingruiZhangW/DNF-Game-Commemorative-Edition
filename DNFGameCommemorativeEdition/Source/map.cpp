#include "map.hpp"
#include "constant.hpp"
#include "floor.hpp"
#include "background.hpp"
#include "floorobj.hpp"

// clang-format off

// Scene one floor tiles.
// 0 for glass, 1 for road, 2 for tree, 3 for normal door, 4 for boss door, 5 for door tree.
// First vector and second vector represents matrix.
// Third vector represents tile map blend at location (arts with alpha channel).
const std::vector<std::vector<std::vector<int>>> scene_one_floor_tiles = {
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0, 2}, {0}},
    {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1, 3}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
    {{0}, {0}, {0}, {0}, {0, 2}, {0}, {0}, {0}, {0}}
};

// Scene one background tiles
// 0 for far, 1 for mid
const std::vector<std::vector<std::vector<int>>> scene_one_background_tiles = {
    {{0, 1}, {0, 1}, {0, 1}, {0, 1}}
};

// clang-format on

Map::Map(ShaderProgram* shader, GLfloat width, GLfloat height)
    : SceneNode(StringContant::sceneOneMapName)
    , m_shader(shader)
    , m_window_width(width)
    , m_window_height(height)
    , m_map_boundary(glm::vec4(0.0f))
    , m_map_top_offset(0.0f)
    , m_map_right_offset(0.0f)
{}

Map::~Map() {}

void
Map::initSceneOneMap()
{
    m_floor_obj_list.clear();

    m_map_boundary.z = 0.0f;

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

                    m_map_boundary.w = m_map_boundary.w + farBack->getTextureGeo().x;
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
    m_map_boundary.x = (m_window_height * RatioContant::floorHeightScaleRatio)
                           * RatioContant::attachOffset
                       - m_map_top_offset;
    m_map_boundary.y = 0;

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

    // Floor obj
    for (size_t i = 0; i < rowNum; i++) {
        for (size_t j = 0; j < colNum; j++) {
            auto heightScale = (m_window_height * RatioContant::floorHeightScaleRatio) / rowNum;

            for (size_t k = 0; k < scene_one_floor_tiles[i][j].size(); k++) {
                if (scene_one_floor_tiles[i][j][k] == 2) {
                    FloorObj* doorTree = new FloorObj(StringContant::sceneOneFloorObjName
                                                          + std::to_string(i + j + k),
                                                      m_shader,
                                                      FloorObj::FloorObjType::Tree);
                    doorTree->translate(
                        glm::vec3(doorTree->getTextureGeo().x * j, heightScale * i, 0.0f));

                    m_floor_obj_list.push_back(
                        std::make_pair(doorTree,
                                       glm::vec2(doorTree->getTextureGeo().x * j, heightScale * i)));
                }
            }
        }
    }

    m_map_boundary.w = m_map_boundary.w - m_map_right_offset;
}

glm::vec4
Map::getMapBoundary()
{
    return m_map_boundary;
}

const std::vector<std::pair<FloorObj*, glm::vec2>>&
Map::getFloorObjs()
{
    return m_floor_obj_list;
}