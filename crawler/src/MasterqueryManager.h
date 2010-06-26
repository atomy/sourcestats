//#ifndef MASTERQUERYMANAGER_H
//#define MASTERQUERYMANAGER_H
//
//#include <vector>
//#include "GameInfoRequest.h"
//
//
//class Masterquery;
//
//class MasterqueryManager
//{
//private:
//    MasterqueryManager() { }
//    MasterqueryManager( const MasterqueryManager& cc );
//    ~MasterqueryManager() { }
//
//    static MasterqueryManager* gMasterqueryManager;
//
//    std::vector <Masterquery*> m_vMasterQueries;
//    std::vector <Masterquery*>::iterator m_mqIt;
//
//public:
//    static			MasterqueryManager* getInstance();
//    static void		Destroy();
//
//    static void*	ThreadGetserversForGame( void* arg );
//
//    void			CreateGameWorker( const char* );
//    Masterquery*	GetFinishedQuery( void );
//    void			ResetIterator( void );
//    void  			AddQuery( Masterquery* pQuery );
//
//
//  private:
//    class MMThreadArgs
//    {
//    public:
//		MMThreadArgs( MasterqueryManager* pObj, const char* pArg ) : pObject( pObj ), pGameName( pArg ) { }
//
//		const char* GetGame( void ) { return pGameName; }
//		MasterqueryManager* GetObj( void ) { return pObject; }
//
//    private:
//		MasterqueryManager* 	pObject;
//		const char* 			pGameName;
//    };
//};
//
//#endif // MASTERQUERYMANAGER_H
