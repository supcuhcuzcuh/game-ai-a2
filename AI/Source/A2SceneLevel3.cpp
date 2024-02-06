#include "A2SceneLevel3.h"
#include "Application.h"

// TODO
bool A2SceneLevel3::PlayerLostDecision()
{
    return false;
}

void A2SceneLevel3::OnPlayerLost()
{
}

void A2SceneLevel3::PlayerDecision()
{
    if (Application::IsMouseJustPressed(0))
    {
        _player->path.clear();
        _world.pathfind(_player, GetMousePosition(), _player->path);
    }
}

void A2SceneLevel3::MapUpdate()
{

    std::vector<int> RandomTiles = _world.getCells(3);

    // Flip the coin using rand() between 1 and 2
    int result = rand() % 2;


    if (result == 1) {
        for (int i = 0; i < RandomTiles.size(); ++i)
        {
            // Create a WorldTile based on the coin flip
            WorldTile& randomTile = _world.getTile(RandomTiles[i]);

            // Set texture and identity for player
            randomTile.texture = playerTexture;
            randomTile.entity = nullptr;
        }
    }
    else {
        
        for (int i = 0; i < RandomTiles.size(); ++i)
        {
            // Create a WorldTile based on the coin flip
            WorldTile& randomTile = _world.getTile(RandomTiles[i]);

            // Set texture and identity for player
            randomTile.texture = wallTexture;
            randomTile.entity = &wall;
        }
    }
}

void A2SceneLevel3::OnMobTouchedPlayer()
{
}

void A2SceneLevel3::Init()
{
    A2BaseScene::Init();

    // Load Textures
    grassTexture = LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles("Image/grass.png");
    wallTexture = LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles("Image/bricks.png");
    enemyTexture = LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles("Image/enemy.jpg");
    playerTexture = LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles("Image/player.jpg");
    exitTexture = LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles("Image/door.jpg");

    // Load Wall Tile
    wall.texture = wallTexture;
    wall.baseAP = 0;
    wall.currentAP = 0;

    // Load Lookup Table
    std::vector<WorldTile> lookup;
    WorldTile grassTile;
    grassTile.texture = grassTexture;
    grassTile.entity = nullptr;
    WorldTile wallTile;
    wallTile.texture = grassTexture;
    wallTile.entity = &wall;

    WorldTile Exit;
    Exit.texture = exitTexture;
    Exit.entity = nullptr;

    WorldTile RandomWall;
    RandomWall.texture = wallTexture;
    RandomWall.entity = &wall;
    // Put Tiles into Lookupo Table
    lookup.push_back(grassTile); // This is push_back'd first so it is index 0 on the CSV
    lookup.push_back(wallTile); // This is push_back'd second so it is index 1 on the CSV
    lookup.push_back(Exit);
    lookup.push_back(RandomWall);

    // Generate the world using the CSV, sizes and lookup table
    _world = World("maps/thirdlevel.csv",20,20, lookup);

    _player = new EntityButSmallerSize;
    _mobs.push_back(_player);
    _player->baseAP = 50;
    _player->currentAP = 50;
    _player->texture = playerTexture;
    _world.addEntity(_player);

    /*
     * EXAMPLE
     * If the CSV Contains numbers that aren't in the lookup table, the world will load the first tile instead
     * To Access these cells to populate them manually (For things like spawning the enemies or special tiles)
     */
    std::vector<int> enemyTiles = _world.getCells(10); // 10 on CSV means enemies in this case
    std::vector<int> doorTiles = _world.getCells(99);

   /* for (int i = 0; i < doorTiles.size(); ++i)
    {
        EntityButSmallerSize* door = new EntityButSmallerSize;
        door->texture = exitTexture;
        _world.addEntity(door, _world.indexToCoordinates(doorTiles[i]));
    }*/

    std::vector<int> ExitTiles = _world.getCells(2);
    for (int i = 0; i <= 1; ++i)
    {

       level1EndPoint = _world.indexToCoordinates(ExitTiles[0]);
    }
    for (int i = 0; i < enemyTiles.size(); ++i)
    {
        // Spawn Entities Here
        EntityButSmallerSize* e = new EntityButSmallerSize;
        _mobs.push_back(e); // Add to the list of mobs
        // Configure Entity Stats
        e->baseAP = 6;
        e->currentAP = 6;
        e->texture = enemyTexture;
        _world.addEntity(e, _world.indexToCoordinates(enemyTiles[i]));
    }

    //potentialCellsToDisappear = _world.getCells() // Store all the cells that have potential to disappear 
}

void A2SceneLevel3::Update(double deltaTime)
{
    A2BaseScene::Update(deltaTime);

    if (_world.tileToCoordinates(_world.getEntityLocation(_player)) == level1EndPoint)
    {
        std::cout << "PLAYER HAS REWACHED END POIMNT SALAASAASK" << std::endl;
    }

    TurnUpdate();
}

void A2SceneLevel3::Render()
{
    A2BaseScene::Render();

    modelStack.PushMatrix();
    modelStack.Translate(0.5, 0.5,0);
    RenderWorld();

    for (int i = 0; i < _player->path.size(); ++i)
    {
        modelStack.PushMatrix();
        modelStack.Translate(_player->path[i].first, _player->path[i].second, 0);
        modelStack.Scale(0.25, 0.25, 1);
        RenderMesh(_square, false);
        modelStack.PopMatrix();
    }
    modelStack.PopMatrix();
}

void A2SceneLevel3::Exit()
{
    A2BaseScene::Exit();
}

A2SceneLevel3::A2SceneLevel3()
{
}
