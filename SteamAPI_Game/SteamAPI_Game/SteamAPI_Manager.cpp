#include "SteamAPI_Manager.h"

SteamAPI_Manager::SteamAPI_Manager()
{
	// Nothing
}

SteamAPI_Manager::~SteamAPI_Manager()
{
	std::cout << "Exiting steam..." << std::endl;
	SteamAPI_Shutdown();
}

bool SteamAPI_Manager::Init()
{
	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		std::cout << "Error Restart [SteamAPI_Manager]" << std::endl;
		return false;
	}

	if (!SteamAPI_Init())
	{
		std::cout << "Error Init [SteamAPI_Manager]" << std::endl;
		return false;
	}

	if (!SteamUser()->BLoggedOn())
	{
		std::cout << "Error Logging [SteamAPI_Manager]" << std::endl;
		return false;
	}

	if (!SteamInput()->Init(false))
	{
		std::cout << "Error SteamInput [SteamAPI_Manager]" << std::endl;
		return false;
	}

	/*char rgchCWD[1024];
	if (!_getcwd(rgchCWD, sizeof(rgchCWD)))
	{
		strcpy_s(rgchCWD, ".");
	}

	char rgchFullPath[1024];
	_snprintf_s(rgchFullPath, sizeof(rgchFullPath), "%s\\%s", rgchCWD, "steam_input_manifest.vdf");

	SteamInput()->SetInputActionManifestFilePath(rgchFullPath);*/

	// Test du Steamworks Interfaces pour afficher le nom de l'utilisateur.
	const char* name = SteamFriends()->GetPersonaName();
	std::cout << "User Name : " << name << std::endl;

	return true;
}

void SteamAPI_Manager::Update()
{
	SteamAPI_RunCallbacks();
}

void SteamAPI_Manager::GetFriends()
{
	int nFriends = SteamFriends()->GetFriendCount(k_EFriendFlagAll);

	if (nFriends == -1)
	{
		std::cout << "GetFriendCount returned -1, the current user is not logged in." << std::endl;
		// We always recommend resetting to 0 just in case you were to do something like allocate
		// an array with this value, or loop over it in a way that doesn't take into the -1 into account.
		nFriends = 0;
	}

	for (int i = 0; i < nFriends; i++)
	{
		CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
		const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
		SteamFriends()->RequestUserInformation(friendSteamID, true);
	}
}



/*std::ifstream f("steam_appid.txt");

	if (f.is_open())
	{
		int appId = int(f.rdbuf());

		if (!SteamApps()->BIsAppInstalled(appId))
		{
			std::cout << "App id : " << appId << "; App not installed" << std::endl;
		}
	}*/