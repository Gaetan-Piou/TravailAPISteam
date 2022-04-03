#include "GameManager.h"

void GameManager::OnFriendStatusChanged(PersonaStateChange_t* pCallback)
{
    const char* friendName = SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamID);
    EPersonaState friendStatusEnum = SteamFriends()->GetFriendPersonaState(pCallback->m_ulSteamID);

    const char* friendStatus;
    switch (friendStatusEnum)
    {
        case k_EPersonaStateOffline:
            friendStatus = "Hors ligne";
            break;
        case k_EPersonaStateOnline:
            friendStatus = "En ligne";
            break;
        case k_EPersonaStateBusy:
            friendStatus = "Occupe";
            break;
        case k_EPersonaStateAway:
            friendStatus = "Absent";
            break;
        case k_EPersonaStateSnooze:
            friendStatus = "Ne pas deranger";
            break;
        case k_EPersonaStateLookingToTrade:
            friendStatus = "Cherche un echange";
            break;
        case k_EPersonaStateLookingToPlay:
            friendStatus = "Veut jouer";
            break;
        case k_EPersonaStateInvisible:
            friendStatus = "Invisible";
            break;
        default:
            friendStatus = "Statut inconnu";
            break;
    }

    std::cout << "Le statut de " << friendName << " est desormais " << friendStatus << " !" << std::endl;

    
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

    std::cout << "Joueurs presents : " << pCallback->m_cPlayers << std::endl << std::endl;
}

void GameManager::OnGetLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure)
{
    if (bIOFailure)
    {
        std::cout << "Error : LobbyMatchList_t failed" << std::endl;
        bSearching = false;
        return;
    }

    std::cout << "Nombre de lobbies : " << pCallback->m_nLobbiesMatching << std::endl;

   for (int i = 0; i < pCallback->m_nLobbiesMatching; i++)
   {
       CSteamID LobbyID = SteamMatchmaking()->GetLobbyByIndex(i);
       if (LobbyID.IsLobby() && SteamMatchmaking()->GetNumLobbyMembers(LobbyID) > 4)
       {
           std::cout << "Numero du lobby : " << i << std::endl;
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
        std::cout << "Nom d'utilisateur : " << userName;

        const char* oldUserName = SteamFriends()->GetFriendPersonaNameHistory(UserID, 0);
        std::cout << "(Anciennement " << oldUserName << ")" << std::endl;
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