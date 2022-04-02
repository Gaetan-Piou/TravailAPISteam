#pragma once
#include "SteamAPI_Manager.h"

class GameManager
{
private:
    SteamAPI_Manager* pSteamManager;

    STEAM_CALLBACK(GameManager, OnFriendStatusChanged, PersonaStateChange_t);

    void OnGetNumberOfCurrentPlayer(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure);
    CCallResult<GameManager, NumberOfCurrentPlayers_t> m_NumberOfCurrentPlayersCallResult;

    void OnGetLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure);
    CCallResult<GameManager, LobbyMatchList_t> m_LobbyMatchListCallResult;

    void OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure);
    CCallResult<GameManager, LobbyEnter_t> m_LobbyEnterCallResult;

    // void OnFriendStatusChanged(PersonaStateChange_t* pCallback); // Inutile

public:
    bool bRunning;
    bool bSearching;

    //Constructeurs
    GameManager();

    //Mutateurs
    void SetSteamManager(SteamAPI_Manager* pSteamManager);

    //Fonctions
    void Update();
};