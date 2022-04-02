#pragma once
#include "..//HeaderFiles/steam_api.h"
#include <direct.h>
#include <iostream>
#include <fstream>

class SteamAPI_Manager
{
private:
	bool bIsOnline;

public:
	SteamAPI_Manager();
	~SteamAPI_Manager();

	bool Init();
	void Update();
	void GetFriends();
};
