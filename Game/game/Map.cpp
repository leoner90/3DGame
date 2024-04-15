#include "Game.h"
#include "headers/Map.h"
#include "headers/Player/Player.h"

Map::Map()
{
	font.LoadDefault();

	//1 barrels model
	barrels.LoadModel("barrel/barrels.obj");
	barrels.SetScale(0.33f);
	barrels.SetToFloor(0);

	//2 Tree
	tree.LoadModel("tree/tree.obj");
	tree.SetScale(65.f);
	tree.SetToFloor(0);

	//3 flowers model
	flowers.LoadModel("flowers/flowers2.obj");
	flowers.SetY(0);
	flowers.SetScale(45.f);

	//4 grass model
	grass.LoadModel("grass/grass.obj");
	grass.SetY(0);
	grass.SetScale(0.1f);

	//5 hidingWall
	hidingWall.LoadModel("hidingWall/hidingWall.obj");
	hidingWall.SetScale(1.6f);
	hidingWall.SetToFloor(0);

	//6 house model
	house.LoadModel("house/house.obj");
	house.SetY(0);
	house.SetScale(6.0f);


	//7 iglo model
	iglo.LoadModel("iglo/iglo.obj");
	iglo.SetY(50);
	iglo.SetScale(55.0f);

	//8 mushroom model
	mushroom.LoadModel("mushroom/MushroomShiitake.obj");
	mushroom.SetScale(3.5f);
	mushroom.SetToFloor(0);


	//9 mountain model
	mountain.LoadModel("mountain/mountain.obj");
	mountain.SetScale(10.0f);
	mountain.SetToFloor(0);

	//10 rock2
	rock2.LoadModel("rock2/rock2.obj");
	rock2.SetScale(55.0f);
	rock2.SetToFloor(0);

	//11 snowman
	snowman.LoadModel("snowman/snowman.obj");
	snowman.SetScale(75.0f);
	snowman.SetToFloor(0);


	//12 wall
	wall.LoadModel("wall/wall.obj");
	wall.SetScale(10.0f);
	wall.SetToFloor(0);



	// floor texture
	//floor.LoadTexture("Map/floor2.bmp");
	floor.LoadTexture("Map/floor.jpg");
	floor.SetTiling(true);


	

	// coin model
	rainDrop.LoadModel("snowFlake/snowFlake.obj");
	rainDrop.SetY(50);
	rainDrop.SetScale(0.35f);
	

	for (int i = 0; i < 500; i++)
	{
		CModel* pShot = rainDrop.Clone();
		pShot->SetColor(CColor::White());
		pShot->SetPositionV(CVector(rand() % 3000, 2000, rand() % 3000));
		rain.push_back(pShot);
	}
}

void Map::init(int level)
{
	collidingObjects.clear();
	collidingObjects.delete_if(true);
	mapCollision.delete_all();
	mapCollision.delete_if(true);
	modelList.delete_all();
	modelList.delete_if(true);

	gameLevel = level;
	LoadData();
 



}

void Map::LoadData()
{
	modelList.clear();
	floor.SetSize(7500, 7500);
	floor.SetPosition(0, 0, 0);

	// move gameworld so that it starts at 0,0 and extends in positive xz-direction
	//floor.SetPosition(floor.GetWidth() / 2, floor.GetDepth() / 2);

	

	//GAME LVL MAP
	fstream myfile;
	if(gameLevel == 1) myfile.open("level1.txt", ios_base::in);
	else if (gameLevel == 2) myfile.open("level2.txt", ios_base::in);
	else if (gameLevel == 3) myfile.open("level3.txt", ios_base::in);

	int type, x, y, z, rot;
	bool neof; // not end of file
	do
	{
		myfile >> type >> x >> y >> z >> rot;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << " " << z << " " << rot << endl;
		if (neof) 
		{
			if (type == 1)   // barrels segments
			{
				CModel* barrel = barrels.Clone(); // clone wall segment
				barrel->SetPosition((float)x, (float)y, (float)z);
				barrel->SetRotation(float(rot));
				barrel->SetToFloor(0);
				barrel->SetStatus(1); // indicate the type of object
				collidingObjects.push_back(barrel);
			}

			if (type == 2)  // trees
			{
				CModel* trees = tree.Clone();
				trees->SetPosition((float)x, (float)y, (float)z);
				trees->SetRotation(float(rot));
				trees->SetRotation((float)rot);
				trees->SetStatus(2); // indicate the type of object
				collidingObjects.push_back(trees);
			}

			if (type == 3) // flowers
			{
				CModel* flower = flowers.Clone();
				flower->SetPosition((float)x, (float)y, (float)z);
				flower->SetRotation(float(rot));
				flower->SetStatus(3); // indicate the type of object
				modelList.push_back(flower);
			}

			if (type == 4) // grass
			{
				CModel* grassobj = grass.Clone();
				grassobj->SetPosition((float)x, (float)y, (float)z);
				grassobj->SetRotation(float(rot));
				grassobj->SetStatus(4); // indicate the type of object
				modelList.push_back(grassobj);
			}

			if (type == 5) // hidingWalls
			{
				CModel* hidingWalls = hidingWall.Clone();
				hidingWalls->SetPosition((float)x, (float)y - 30, (float)z);
				hidingWalls->SetRotation((float)rot);
				hidingWalls->SetStatus(5); // indicate the type of object
				collidingObjects.push_back(hidingWalls);
			}

			if (type == 6) // house
			{
				CModel* houses = house.Clone();
				houses->SetPosition((float)x, (float)y, (float)z);
				houses->SetRotation(float(rot));
				houses->SetStatus(6); // indicate the type of object
				collidingObjects.push_back(houses);
			}

			if (type == 7) // iglo
			{
				CModel* igloobj = iglo.Clone();
				igloobj->SetPosition((float)x, (float)y - 60, (float)z);
				igloobj->SetRotation(float(rot));
				igloobj->SetStatus(7); // indicate the type of object
				collidingObjects.push_back(igloobj);
			}

			if (type == 8) // mushroom
			{
				CModel* mushrooms = mushroom.Clone();
				mushrooms->SetPosition((float)x, (float)y, (float)z);
				mushrooms->SetRotation(float(rot));
				mushrooms->SetStatus(8); // indicate the type of object
				modelList.push_back(mushrooms);
			}

			if (type == 9) // mountains
			{
				CModel* mountains = mountain.Clone();
				mountains->SetPosition((float)x, (float)y, (float)z);
				mountains->SetRotation(float(rot));
				mountains->SetStatus(9); // indicate the type of object
				modelList.push_back(mountains);
			}

			if (type == 10) // rock
			{
				CModel* rock = rock2.Clone();
				rock->SetPosition((float)x, (float)y, (float)z);
				rock->SetRotation(float(rot));
				rock->SetStatus(10); // indicate the type of object
				collidingObjects.push_back(rock);
			}

			if (type == 11) // snowman
			{
				CModel* snowmanObj = snowman.Clone();
				snowmanObj->SetPosition((float)x, (float)y, (float)z);
				snowmanObj->SetRotation(float(rot));
				snowmanObj->SetStatus(11); // indicate the type of object
				collidingObjects.push_back(snowmanObj);
			}

			if (type == 12) // wall
			{
				CModel* wallsColliders = wall.Clone();
				wallsColliders->SetPosition((float)x, (float)y, (float)z);
				wallsColliders->SetRotation(float(rot));
				wallsColliders->SetStatus(12); // indicate the type of object
				mapCollision.push_back(wallsColliders);
			}
		}
	} while (neof);
	myfile.close();
}

void Map::OnUpdate(Uint32 t, Player& player)
{
	localTime = t;
	localPlayer = &player;
	weather();
	modelList.Update(t);
}

void Map::OnDraw(CGraphics* g) {   }


void Map::OnRender3D(CGraphics* g)
{
	//mapCollision.Draw(g);
	collidingObjects.Draw(g);
	floor.Draw(g);
	modelList.Draw(g);
	rain.Draw(g);
}

void Map::weather()
{
	for (auto drop : rain)
	{
		float y = drop->GetPositionV().GetY();
		float x = localPlayer->playerModel.GetX() + (rand() % 1500) - (rand() % 1500);
		float z = localPlayer->playerModel.GetZ() + (rand() % 1000) - (rand() % 1000);

		if (y < 50)
		{
			CVector pos = (CVector(x, 2000, z));
			drop->SetPositionV(pos);
		}
		else
		{
			y -= rand() % 80; //speed
			drop->SetPosition(drop->GetX(), y, drop->GetZ());
		}
	}
	rain.Update(localTime);
}
