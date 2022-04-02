#include "GameManager.h"

void GameManager::OnFriendStatusChanged(PersonaStateChange_t* pCallback)
{
    const char* friendName = SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamID);
    EPersonaState friendStatusEnum = SteamFriends()->GetFriendPersonaState(pCallback->m_ulSteamID);

    const char* friendStatus;
    switch (friendStatusEnum)
    {
        case k_EPersonaStateOffline:
            friendStatus = "offline";
            break;
        case k_EPersonaStateOnline:
            friendStatus = "online";
            break;
        case k_EPersonaStateBusy:
            friendStatus = "busy";
            break;
        case k_EPersonaStateAway:
            friendStatus = "away";
            break;
        case k_EPersonaStateSnooze:
            friendStatus = "set on snooze";
            break;
        case k_EPersonaStateLookingToTrade:
            friendStatus = "looking to trade";
            break;
        case k_EPersonaStateLookingToPlay:
            friendStatus = "looking to play";
            break;
        case k_EPersonaStateInvisible:
            friendStatus = "invisible";
            break;
        default:
            friendStatus = "unkown status";
            break;
    }

    std::cout << friendName << " status is now " << friendStatus << " !" << std::endl;

    
    SteamAPICall_t hSteamAPICall = SteamUserStats()->GetNumberOfCurrentPlayers();
    m_NumberOfCurrentPlayersCallResult.Set(hSteamAPICall, this, &GameManager::OnGetNumberOfCurrentPlayer);
}

void GameManager::OnGetNumberOfCurrentPlayer(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure)
{
    if (bIOFailure || !pCallback->m_bSuccess)
    {
        std::cout << "Error : NumberOfCurrentPlayers_t failed" << std::endl;
        return;
    }

    std::cout << "Numbers of players currently playing : " << pCallback->m_cPlayers << std::endl << std::endl;
}

void GameManager::OnGetLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure)
{
    if (bIOFailure)
    {
        std::cout << "Error : LobbyMatchList_t failed" << std::endl;
        bSearching = false;
        return;
    }

    std::cout << "Number of lobbies matching : " << pCallback->m_nLobbiesMatching << std::endl;

   for (int i = 0; i < pCallback->m_nLobbiesMatching; i++)
   {
       CSteamID LobbyID = SteamMatchmaking()->GetLobbyByIndex(i);
       if (LobbyID.IsLobby() && SteamMatchmaking()->GetNumLobbyMembers(LobbyID) > 4)
       {
           std::cout << "Lobby number : " << i << std::endl;
           SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(LobbyID);
           m_LobbyEnterCallResult.Set(hSteamAPICall, this, &GameManager::OnLobbyEntered);
           break;
       }
   }
}

void GameManager::OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure)
{
    int nb = SteamMatchmaking()->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby);
    for (int i = 0; i < nb; i++)
    {
        CSteamID UserID = SteamMatchmaking()->GetLobbyMemberByIndex(pCallback->m_ulSteamIDLobby, i);

        const char* userName = SteamFriends()->GetFriendPersonaName(UserID);
        std::cout << "User Name : " << userName << std::endl;

        const char* oldUserName = SteamFriends()->GetFriendPersonaNameHistory(UserID, 0);
        std::cout << "Old User Name : " << oldUserName << std::endl;
    }
}

GameManager::GameManager()
{
    bRunning = true;
    bSearching = false;
    pSteamManager = nullptr;
}

void GameManager::SetSteamManager(SteamAPI_Manager* pSteamManager)
{
    this->pSteamManager = pSteamManager;
}

void GameManager::Update()
{
    while (bRunning)
    {
        if (pSteamManager != nullptr)
        {
            pSteamManager->Update();
            pSteamManager->GetFriends();
        }

        if (!bSearching)
        {
            bSearching = true;
            SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
            m_LobbyMatchListCallResult.Set(hSteamAPICall, this, &GameManager::OnGetLobbyMatchList);
        }

        // SteamAPI_ManualDispatch_Init();
    }
}