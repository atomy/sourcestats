//#ifndef GAMESERVERMANAGER_H
//#define GAMESERVERMANAGER_H
//
//#include "GameserverInfo.h"
//#include "GameInfoRequest.h"
//#include <vector>
//
//class GameserverManager
//{
//private:
//    GameserverManager() { }
//    GameserverManager( const GameserverManager& cc );
//    ~GameserverManager() { }
//
//    static GameserverManager*	gGameserverManager;
//
//	std::vector <GameserverEntry*>	m_vServerEntries;
//	std::vector <GameserverInfo*>	m_vServerInfos;
//	std::vector <GameInfoRequest*>	m_vInfoRequests;
//	std::vector <GameInfoRequest*>::iterator		m_itGIReq;
//
//    static void			CleanupServerInfos( void );
//    void				RequestServInfo( servAddr );
//
//    class MMThreadArgs
//    {
//    public:
//        MMThreadArgs( GameserverManager* pObj, GameInfoRequest* pArg ) : pObject( pObj ), pInfoRequest( pArg ) { }
//
//        GameInfoRequest* GetRequest( void ) {
//            return pInfoRequest;
//        }
//        GameserverManager* GetObj( void ) {
//            return pObject;
//        }
//
//    private:
//        GameserverManager* 	pObject;
//        GameInfoRequest* 	pInfoRequest;
//    };
//
//public:
//    static				GameserverManager* getInstance();
//    static void			Destroy();
//
//    static void*		ThreadGetASInfo( void *arg );
//
//    int					GetServerCount( void );
//    void				AddResult( GameserverInfo* );
//    void				RequestAllServInfo( void );
//    void				AddEntry( GameserverEntry* pEntry );
//    void				Init( void );
//    void				ExecPendingInfos( void );
//    void 				CreateASWorker( GameInfoRequest* pRequest );
//    void				ResetIterator( void );
//    GameInfoRequest*	GetFinishedQuery( void );
//};
//
//#endif // GAMESERVERMANAGER_H
