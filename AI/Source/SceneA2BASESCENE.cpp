#include "SceneA2BASESCENE.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "MeshBuilder.h"

void SceneA2BASESCENE::RenderWorld()
{
    const int count = _world.getWorldSizeLinear();
    for (int i = 0; i < count; ++i)
    {
        auto position = _world.indexToCoordinates(i);
        auto tile = _world.getTile(i);
        _square->textureID = tile.texture;
        modelStack.PushMatrix();
        modelStack.Translate(position.first,position.second,0);
        modelStack.Rotate(180,0,0,1);
        RenderMesh(_square,false);
        if (tile.entity)
        {
            _square->textureID = tile.entity->texture;
            RenderMesh(_square,false);
        }
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
}

void SceneA2BASESCENE::RenderWorldWithFog()
{
    const int radius = 3;
    //int start = _maze.coordToIndex(_maze.getEntityPosition(_player));
    vec2 start = _world.tileToCoordinates(_world.getEntityLocation(_player));
    for (int i = 0; i < radius; ++i)
    {
        start = _world.left(start);
    }
    for (int j = 0; j < radius; ++j)
    {
        start = _world.up(start);
    }
    for (int i = 0; i < radius*2+1; ++i)
    {
        //int col = start;
        vec2 col = start;
        for (int j = 0; j < radius*2+1; ++j)
        {
            //auto pos = _maze.indexToCoord(col);
            auto pos = col;
            if (!_world.withinWorld(pos))
            {
                col = _world.down(col);
                continue;
            }
            auto tile = _world.getTile(_world.coordinateToIndex(pos));
            //auto tile = _maze[col];
            _square->textureID = tile.texture;
            modelStack.PushMatrix();
            modelStack.Translate(pos.first,pos.second,0);
            modelStack.Rotate(180,0,0,1);
            RenderMesh(_square,false);
            if (tile.entity)
            {
                _square->textureID = tile.entity->texture;
                RenderMesh(_square,false);
            }
            modelStack.PopMatrix();
            _square->textureID = 0;
            col = _world.down(col);
        }
        start = _world.right(start);
    }
}

void SceneA2BASESCENE::RenderCourseLine(const std::vector<vec2>& course)
{
}

void SceneA2BASESCENE::MoveCamera(const Vector3& offset)
{
    // Change Camera Values
    camera.position += offset;
    camera.target += offset;

    // Recalculate based off new values
    Mtx44 viewMatrix;
    viewMatrix.SetToLookAt(
            UnpackVector(camera.position),
            UnpackVector(camera.target),
            UnpackVector(camera.up)
    );
    viewStack.LoadMatrix(viewMatrix);
}

vec2 SceneA2BASESCENE::GetMousePosition()
{
    static double x_tile_scale = Application::GetWindowWidth() / _cameraViewRange;
    static double y_tile_scale = Application::GetWindowHeight() / (_cameraViewRange*_aspectRatio);

    double x{},y{};
    Application::GetCursorPos(&x,&y);
    y = _windowHeight - y;

    x /= x_tile_scale;
    x += camera.position.x;
    y /= x_tile_scale;
    y += camera.position.y;
    return vec2{static_cast<int>(x),static_cast<int>(y)};
}

#include "stb_image.h"

unsigned SceneA2BASESCENE::LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles(const char* file)
{
    int x,y,n; // Store the width(x), height(y) and color channels(n) of the image (Color Channels means if it has transparency or not (RGB or RGBA)
    unsigned char* pixel_data = stbi_load(file,&x,&y,&n,4); // Load the image from file (Auto detects file type)
    if (!pixel_data) // Error check if failed to load image
    {
        std::cerr << "Failed To Load Image | " << file << std::endl;
        std::cerr << stbi_failure_reason() << std::endl;
        return 0;
    }

    // Upload PixelData to the GPU as a Texture
    GLuint textureID = 0;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D,textureID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,x,y,0,GL_RGBA,GL_UNSIGNED_BYTE,pixel_data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    float maxAnisotropy = 1.f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)maxAnisotropy );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap( GL_TEXTURE_2D );

    stbi_image_free(pixel_data); // Delete Image from CPU since it is in the GPU now

    return textureID; // Give back
}

// Returns True if the turn has finished
bool SceneA2BASESCENE::DoNextTurn()
{
    static double timer = 0.0;

    EntityButSmallerSize* entity = _mobs[_currentTurn];

    if (!entity)
    {
        DEBUGLOG("This Entity is nullptr");
        return true;
    }
    if (entity->path.empty())
        return true;

    timer += Application::DELTATIME;
    if (timer > entity->speed)
    {
        vec2 tilePos = *(entity->path.end()-1);
        auto index = _world.coordinateToIndex(tilePos);
        const auto& tile = _world.getTile(index);

        if (entity->currentAP < tile.cost)
            entity->path.clear(); // End Turn;
        else
        {
            if (entity != _player)
            {
                if (tilePos == _world.indexToCoordinates(_world.tileToIndex(_world.getEntityLocation(_player))))
                {
                    // The Entity has Touched the Player
                    OnMobTouchedPlayer();
                }
            }
            timer = 0;
            _world.teleportEntity(entity,tilePos);
            entity->path.erase(entity->path.end()-1);
            entity->currentAP -= tile.cost;
        }
    }

    return entity->path.empty();
}

#define LOGVEC2(vec2) std::cout << vec2.first << ',' << vec2.second << std::endl

// Returns True if the turn cycle has repeated
bool SceneA2BASESCENE::CycleTurn()
{
    // Reset Action Points
    EntityButSmallerSize* m = _mobs[_currentTurn];
    m->currentAP = m->baseAP;
    // Clear their Path
    m->path.clear();
    // Cycling Turn
    size_t old = _currentTurn;
    ++_currentTurn;
    _currentTurn %= _mobs.size();

    if (m != _player)
    {
        WorldTile* tile = _world.raycast(
            _world.tileToCoordinates(_world.getEntityLocation(m)),
            _world.tileToCoordinates(_world.getEntityLocation(_player)), { m }
        ).hit;
        if (tile && tile->entity == _player)
        {
            LOGVEC2(_world.tileToCoordinates(_world.getEntityLocation(m)));
            LOGVEC2(_world.tileToCoordinates(tile));
            bool good = _world.pathfind(
                m,
                _world.tileToCoordinates(tile),
                m->path, { _player });
         
            
            std::cout << m->path.size() << std::endl;
        }
        else
        {
            std::cout << "WANDERING" << std::endl;
            // Generate random coordinates within a 20x20 grid
            int newX = rand() % 20;
            int newY = rand() % 20;


            _world.pathfind(_mobs[_currentTurn], vec2(newX, newY), _mobs[_currentTurn]->path);
            // Update the entity's position to the randomly generated coordinates
           // _world.setEntityLocation(m, _world.coordinatesToTile({ newX, newY }));

            std::cout << "WANDERING TO POSITION: (" << newX << ", " << newY << ")" << std::endl;
        }
    }
    // This was used to calculate the path for the mobs | Do in statemachines instead
    // if (_mobs[_currentTurn] != _player)
    //     _world.pathfind(_mobs[_currentTurn],_world.getEntityLocation(_player),_mobs[_currentTurn]->path);
    DEBUGLOG("TURN: " << _currentTurn);

    return old > _currentTurn;
}

void SceneA2BASESCENE::TurnUpdate()
{
    if (_gameOver) return;

    if (_playerTurn)
    {
        PlayerDecision();

        // End Turn
        if (Application::IsMouseJustPressed(1))
        {
            _playerTurn = false;
            DEBUGLOG("Player Ended Turn");
        }
    }
    else
    {
        if (DoNextTurn()) // Is Turn Done
        {
            DEBUGLOG("Turn Ended");
            if (CycleTurn()) // Has Turn Cycled
            {
                DEBUGLOG("Player's Turn");
                _playerTurn = true; // Player's Turn
                _timer += 1;
                MapUpdate();
            }
        }
    }
    if (PlayerLostDecision())
    {
        OnPlayerLost();
        _gameOver = true;
    }
}

void SceneA2BASESCENE::Init()
{
    SceneBase::Init();

    _windowWidth = Application::GetWindowWidth();
    _windowHeight = Application::GetWindowHeight();

    _aspectRatio = (float)_windowHeight/(float)_windowWidth;
    // Initializing Projection Matrix
    Mtx44 projectionMatrix{};
    projectionMatrix.SetToOrtho(
            0,_cameraViewRange,
            0,_cameraViewRange*_aspectRatio,
            -10,10
    );
    projectionStack.LoadIdentity();
    projectionStack.LoadMatrix(projectionMatrix);

    camera.Init({0,0,1},
                {0,0,0},
                {0,1,0});

    Mtx44 viewMatrix{};
    viewMatrix.SetToLookAt(
            UnpackVector(camera.position),
            UnpackVector(camera.target),
            UnpackVector(camera.up)
    );
    viewStack.LoadIdentity();
    viewStack.LoadMatrix(viewMatrix);

    _square = MeshBuilder::GenerateQuad("Square",{1,1,1}); // Generic Square Mesh
}

void SceneA2BASESCENE::Update(double deltaTime)
{

    // Debug Camera Movement
    float camSpeed = static_cast<float>(deltaTime) * 40.f;
    if (Application::IsKeyPressed(GLFW_KEY_A))
        MoveCamera({-camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_D))
        MoveCamera({camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_S))
        MoveCamera({0,-camSpeed,0});
    if (Application::IsKeyPressed(GLFW_KEY_W))
        MoveCamera({0,camSpeed,0});
}

void SceneA2BASESCENE::Render()
{
    SceneBase::Render();
    glDisable(GL_DEPTH_TEST);
}

void SceneA2BASESCENE::Exit()
{
    SceneBase::Exit();
    delete _square;
}

SceneA2BASESCENE::SceneA2BASESCENE(): _basePlayerAP(0)
{
}
