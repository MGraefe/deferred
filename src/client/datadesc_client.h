// client/datadesc_client.h
// client/datadesc_client.h
// client/datadesc_client.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//datadesc_client.h

#pragma once
#ifndef deferred__client__datadesc_client_H__
#define deferred__client__datadesc_client_H__

#include <renderer/datadesc_renderer.h>
#include <renderer/CRenderEntity.h>



typedef IEntityFactoryEditor<CRenderEntity>::EditorFactoryMap EditorFactoryMapClient;

EditorFactoryMapClient &GetEditorFactoryMapClient( void );
const IEntityFactoryEditor<CRenderEntity> *GetEditorFactoryClient( const std::string name );


#define LINK_ENTITY_TO_CLASSNAME_CLIENT( Class, ClassName ) \
	static const CEntityFactoryEditorClient<Class> Class##_EditorFactory( &GetEditorFactoryMapClient(), #ClassName ); \
	const char *Class::GetEntityClass() const { return #ClassName; }

#define LINK_ENTITY_TO_CLASSNAME_CLIENT_RENDERER( Class, ClassName ) \
	static const CEntityFactoryEditorClient<Class> Class##_EditorFactory( &GetEditorFactoryMapClient(), #ClassName );

template<typename T>
class CEntityFactoryEditorClient : public IEntityFactoryEditor<CRenderEntity>
{
public:
	CEntityFactoryEditorClient( EditorFactoryMapClient *map, const std::string &editorName ) :
		IEntityFactoryEditor<CRenderEntity>::IEntityFactoryEditor(map, editorName)
	{
		//T::InitDatadescListEditor();
	}

	virtual CRenderEntity *CreateEntity( CScriptSubGroup *pScript, int index ) const
	{
		T *pEnt = new T(index);
		if( pEnt->GetEntityClass() == NULL )
			error_fatal("No entity classname defined in renderer for client entity %s", GetEntityClassName().c_str());
		else if( strcmp(pEnt->GetEntityClass(), GetEntityClassName().c_str()) != 0 )
			error_fatal("Wrong links of entity classname's in client and renderer.\nClient entity: %s, Renderer entity: %s",
				GetEntityClassName().c_str(), pEnt->GetEntityClass());

		if( pScript )
		{
			TypedDatadescList *pDataDescList = pEnt->GetDatadescListEditor();
			pDataDescList->ReadFromScript( pScript, (void*)pEnt );
			pEnt->OnDatadescFinished();
		}

		return pEnt;
	}
};



#endif // deferred__client__datadesc_client_H__
