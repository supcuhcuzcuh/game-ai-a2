#define NOMINMAX

#include "SceneAssignmentCopyThis.h"

#include "Application.h"
#include "MeshBuilder.h"

#include <algorithm>
#include <GL/glew.h>

#include "Entity.h"
#include "Messages.h"

#include <queue>
#include <limits>

// This #define is required to enable the ".cpp" file that is integrated in the header
#define STB_IMAGE_IMPLEMENTATION
#include "PostOffice.h"
#include "stb_image.h"
#include "Entities/GenericWanderEntity.h"

unsigned SceneAssignmentCopyThis::testEntityTexture;
unsigned SceneAssignmentCopyThis::testProjectileTexture;

void SceneAssignmentCopyThis::despawnEntity(Entity* ent)
{
    auto iter = std::find(_entities.begin(),_entities.end(),ent);
    delete (*iter); // Deallocate Entity
    _entities.erase(iter);
}

void SceneAssignmentCopyThis::RenderEntity(Entity* ent)
{
    const static float s_tileSize = Application::GetWindowHeight() / _mazeSize;

    modelStack.PushMatrix();
    auto pos = ent->position;
    modelStack.Scale(s_tileSize,s_tileSize,1);
    modelStack.Translate(0.5f,0.5f,0);
    modelStack.Translate(pos.x,pos.y,ent->drawLayer);
        
    // Swap Textures
    _quad->textureID = ent->texture;
    RenderMesh(_quad,false);
    _quad->textureID = 0;
    modelStack.PopMatrix();
}

void SceneAssignmentCopyThis::RenderMaze()
{
    const static float s_tileSize = Application::GetWindowHeight() / _mazeSize;
    
    auto mazeData = _maze.getGrid();
    for (unsigned i = 0; i < _maze.GetSize(); ++i)
    {
        for (unsigned j = 0; j < _maze.GetSize(); ++j)
        {
            modelStack.PushMatrix();

            modelStack.Scale(s_tileSize,s_tileSize,1);
            modelStack.Translate(0.5f,0.5f,0);
            // modelStack.Translate(0,19,0);
            modelStack.Translate(static_cast<int>(i),static_cast<int>(j),0);
            _quad->textureID = _tileLookup
            [
                mazeData
                [
                    _maze.convertPointToIndex({static_cast<int>(i),static_cast<int>(j)})
                ]
            ];
            // _quad->textureID = _tileLookup[Maze::TILE_FOG];
            RenderMesh(_quad,false);
            _quad->textureID = 0;
            
            modelStack.PopMatrix();
        }
    }
}

unsigned SceneAssignmentCopyThis::LoadAnyKindOfImage(const char* file)
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

// Return a path using BFS (Breadth First Search)
bool SceneAssignmentCopyThis::pathfind(MazePt start, MazePt end, std::vector<MazePt>& output)
{
    output.clear();
    std::queue<MazePt> pathQueue;
    std::vector<MazePt> previous(_mazeSize*_mazeSize);
    std::vector<bool> visited(_mazeSize*_mazeSize,false);
    auto mazeData = _maze.getGrid();
    
    pathQueue.push(start);
    while (!pathQueue.empty())
    {
        MazePt curr = pathQueue.front();
        pathQueue.pop();
        if (curr.x == end.x && curr.y == end.y)
        {
            while (!(curr.x == start.x && curr.y == start.y))
            {
                output.insert(output.begin(), curr);
                curr = previous[_maze.convertPointToIndex({curr.x,curr.y})];
            }
            output.insert(output.begin(), curr);
            return true;
        }
        //UP
        if (curr.y < _mazeSize - 1)
        {
            MazePt next(curr.x, curr.y + 1);
            if (!visited[next.y * _mazeSize + next.x] && mazeData[next.y * _mazeSize + next.x] == Maze::TILE_EMPTY)
            {
                previous[next.y * _mazeSize + next.x] = curr;
                pathQueue.push(next);
                visited[next.y * _mazeSize + next.x] = true;
            }
        }
        //DOWN
        if (curr.y > 0)
        {
            MazePt next(curr.x, curr.y - 1);
            if (!visited[next.y * _mazeSize + next.x] && mazeData[next.y * _mazeSize + next.x] == Maze::TILE_EMPTY)
            {
                previous[next.y * _mazeSize + next.x] = curr;
                pathQueue.push(next);
                visited[next.y * _mazeSize + next.x] = true;
            }
        }
        //LEFT
        if (curr.x > 0)
        {
            MazePt next(curr.x - 1, curr.y);
            if (!visited[next.y * _mazeSize + next.x] && mazeData[next.y * _mazeSize + next.x] == Maze::TILE_EMPTY)
            {
                previous[next.y * _mazeSize + next.x] = curr;
                pathQueue.push(next);
                visited[next.y * _mazeSize + next.x] = true;
            }
        }
        //RIGHT
        if (curr.x < _mazeSize - 1)
        {
            MazePt next(curr.x + 1, curr.y);
            if (!visited[next.y * _mazeSize + next.x] && mazeData[next.y * _mazeSize + next.x] == Maze::TILE_EMPTY)
            {
                previous[next.y * _mazeSize + next.x] = curr;
                pathQueue.push(next);
                visited[next.y * _mazeSize + next.x] = true;
            }
        }
    }
    return false;
}

void SceneAssignmentCopyThis::Init()
{
    SceneBase::Init();
	
    // Initializing Projection Matrix
    _projectionMatrix.SetToOrtho(
            0,Application::GetWindowWidth(),
            0,Application::GetWindowHeight(),
            -10,10
    );
    projectionStack.LoadIdentity();
    projectionStack.LoadMatrix(_projectionMatrix);

    camera.Init({0,0,1},
                {0,0,0},
                {0,1,0});

    _viewMatrix.SetToLookAt(
    camera.position.x,camera.position.y,camera.position.z,
    camera.target.x,camera.target.y,camera.target.z,
    camera.up.x,camera.up.y,camera.up.z
    );
    viewStack.LoadIdentity();
    viewStack.LoadMatrix(_viewMatrix);

	_quad = MeshBuilder::GenerateQuad(
            "QuadMesh",
            {1,1,1});

    /*
     * Initialize the world (Maze)
     * Maze size 50x50
     * IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
     * Go to Maze.cpp "Generate" Function and add tiles to your world
     * IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
     */
    _maze.Generate(time(nullptr),_mazeSize,{0,0},0);

    // Loading Tile Textures
    _tileLookup[Maze::TILE_WALL] = LoadAnyKindOfImage("Image/breen2.png");
    _tileLookup[Maze::TILE_FOG] = LoadAnyKindOfImage("Image/breen1.jpg");
    // Not loading a texture for TILE_EMPTY because its supposed to be empty
    _tileLookup[Maze::TILE_EMPTY] = 0;

    testEntityTexture = LoadAnyKindOfImage("Image/losestate.jpg");
    testProjectileTexture = LoadAnyKindOfImage("Image/jeraldo.png");

    PostOffice::GetInstance()->Register("MainScene",this);
    
    // Pathfinding Testing
    std::vector<MazePt> route;
    bool routable = pathfind({0,0},{10,10},route);
    if (!routable)
        std::cerr << "Pathfinder cannot find a route" << std::endl;
    else
    {
        unsigned char tempMap[20 * 20];
        for (int i = 0; i < 20 * 20; ++i)
            tempMap[i] = 0;
        for (auto pt : route)
            tempMap[_maze.convertPointToIndex(pt)] = 1;
        auto mazeData = _maze.getGrid();
        for (int i = 0; i < _mazeSize; ++i)
        {
            for (int j = 0; j < _mazeSize; ++j)
            {
                if (mazeData[_maze.convertPointToIndex({i,j})] == Maze::TILE_WALL)
                    std::cout << 'M' << '-';
                else
                    std::cout << static_cast<int>(tempMap[_maze.convertPointToIndex({i, j})]) << '-';
            }
            std::cout << std::endl;
        }
    }
    // Pathfinding Testing

    testEntity = spawnEntity<GenericWanderEntity>();
    testEntity->texture = testEntityTexture;
}

void SceneAssignmentCopyThis::Update(double dt)
{
    for (int i = 0; i < _entities.size(); ++i)
        _entities[i]->update(dt);

    // Handle Spawning Entities during Loop Period (From StateMachines)
    for (auto& birth : birthRow)
        _entities.push_back(birth);
    // Handle De-Spawning Entities during Loop Period (From StateMachines)
    for (auto& dead : deathRow)
        despawnEntity(dead);

    birthRow.clear();
    deathRow.clear();
}

void SceneAssignmentCopyThis::Render()
{
    SceneBase::Render();
    modelStack.Clear();
    // Render Background
    // modelStack.PushMatrix();
    // modelStack.Scale(
    //     Application::GetWindowHeight(),
    //     Application::GetWindowHeight(),
    //     1);
    // modelStack.Translate(0.5f,0.5f,0);
    // RenderMesh(_quad,false);
    // modelStack.PopMatrix();

    // Render the Maze
    RenderMaze();
    
    // Render All Entities
    for (auto ent : _entities)
    {
        RenderEntity(ent);
    }
}

void SceneAssignmentCopyThis::Exit()
{
}

bool SceneAssignmentCopyThis::Handle(Message* message)
{
    // Handle Messages Here

    // Message to Ask Scene to Pathfind for me
    // Dynamic Cast to check if the message received is this particular message
    PathfindMessage* pathfind_message = dynamic_cast<PathfindMessage*>(message);
    if (pathfind_message) // if not nullptr then it is otherwise no
    {
        // Pathfind for the Sender
        pathfind_message->gotRoute = pathfind(pathfind_message->start,pathfind_message->end,pathfind_message->route);
        return true;
    }
    GetRandomEmptyPositionMessage* get_random_empty_position_message = dynamic_cast<GetRandomEmptyPositionMessage*>(message);
    if (get_random_empty_position_message)
    {
        auto mazeData = _maze.getGrid();
        int x = rand() % _mazeSize;
        int y = rand() % _mazeSize;
        while (!mazeData[_maze.convertPointToIndex({x,y})] == Maze::TILE_EMPTY)
        {
            x = rand() % _mazeSize;
            y = rand() % _mazeSize;
        }
        get_random_empty_position_message->x = x;
        get_random_empty_position_message->y = y;
        return true;
    }
    GetRandomEmptyPositionSubSectionMessage* get_random_empty_position_sub_section_message = dynamic_cast<GetRandomEmptyPositionSubSectionMessage*>(message);
    if (get_random_empty_position_sub_section_message)
    {
        auto start = get_random_empty_position_sub_section_message->bottomLeft;
        auto end = get_random_empty_position_sub_section_message->topRight;
        auto mazeData = _maze.getGrid();
        int x = (rand() % (end.x - start.x)) + start.x;
        int y = (rand() % (end.y - start.y)) + start.y;
        while (!mazeData[_maze.convertPointToIndex({x,y})] == Maze::TILE_EMPTY)
        {
            x = (rand() % (end.x - start.x)) + start.x;
            y = (rand() % (end.y - start.y)) + start.y;
        }
        get_random_empty_position_sub_section_message->x = x;
        get_random_empty_position_sub_section_message->y = y;
        return true;
    }
    FindNearestEntity* find_nearest_entity = dynamic_cast<FindNearestEntity*>(message);
    if (find_nearest_entity)
    {
        float distance = std::numeric_limits<float>::max();
        Entity* nearest = nullptr;
        for (const auto& ent : _entities)
        {
            if (ent == find_nearest_entity->who_asked) continue;
            if (ent->type != find_nearest_entity->type) continue;
            const auto& pos = ent->position;
            float nextDistance = (pos.x * pos.x) + (pos.y * pos.y); // Pythagoras Theorem
            if (distance > nextDistance)
            {
                distance = nextDistance;
                nearest = ent;
            }
        }
        find_nearest_entity->nearest = nearest;
        return true;
    }
    SpawnEntityMessage* spawn_entity_message = dynamic_cast<SpawnEntityMessage*>(message);
    if (spawn_entity_message)
    {
        birthRow.push_back(spawn_entity_message->toIntroduceIntoTheScene);
        return true;
    }
    KillEntityMessage* kill_entity_message = dynamic_cast<KillEntityMessage*>(message);
    if (kill_entity_message)
    {
        auto iter = std::find(_entities.begin(),_entities.end(),kill_entity_message->deathRow);
        if (iter == _entities.end()) // Entity does not exist in the Scene so it is probably already dead
            return true;
        deathRow.push_back(kill_entity_message->deathRow);
        return true;
    }

    return false;
}
