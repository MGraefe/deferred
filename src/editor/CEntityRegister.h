// editor/CEntityRegister.h
// editor/CEntityRegister.h
// editor/CEntityRegister.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CEntityRegister.h

#pragma once
#ifndef deferred__editor__CEntityRegister_H__
#define deferred__editor__CEntityRegister_H__

enum paramtypes_e
{
	PT_UNKNOWN = 0,
	PT_ANGLE ,
	PT_ORIGIN,
	PT_COLOR,
	PT_LAST,
};


paramtypes_e GetParameterType( const std::string &type );


class CEntityRegisterOutput
{
public:
	CEntityRegisterOutput(const std::string &name,
		const std::string &desc)
	{
		this->name = name;
		this->desc = desc;
	}

	inline bool operator<(const CEntityRegisterOutput &other)
	{
		return name.compare(other.name) < 0;
	}
public:
	std::string name;
	std::string desc;

};


typedef CEntityRegisterOutput CEntityRegisterInput;


class CEntityKeyValue
{
public:
	CEntityKeyValue( const std::string &name,
		const std::string &desc,
		const std::string &val,
		paramtypes_e type )
	{
		this->name = name;
		this->desc = desc;
		this->val = val;
		this->type = type;
	}

	inline bool operator<(const CEntityRegisterOutput &other)
	{
		return name.compare(other.name) < 0;
	}
public:
	std::string name;
	std::string desc;
	std::string val;
	paramtypes_e type;
};

class CEntityRegister;

class CEntityRegisterEntity
{
	friend class CEntityRegister;
public:
	const std::string &GetClassname( void ) const { return m_classname; }
	const std::string &GetDescribtion(void) const { return m_describtion; }
	CEntityRegisterEntity( const std::string &classname );
	CEntityKeyValue *Get( const std::string &name ) const;
	CEntityKeyValue *Get( const CString &name ) const;
	CEntityKeyValue *Get( const char *name ) const;
	CEntityKeyValue *Get( int i ) const;
	const std::string &GetBaseVal(const string &key);
	inline int size( void ) { return m_KeyVals.size(); }

	inline size_t GetNumOutputs() { return m_Outputs.size(); }
	inline size_t GetNumInputs() { return m_Inputs.size(); }
	inline const CEntityRegisterInput *GetInput(int i) { return &(m_Inputs[i]); }
	inline const CEntityRegisterOutput *GetOutput(int i) { return &(m_Outputs[i]); }
	//inline const std::vector<CEntityRegisterInput> &GetInputs() { return m_Inputs; }
	//inline const std::vector<CEntityRegisterOutput> &GetOutputs() { return m_Outputs; }

	void AddKeyVal( const std::string &name,
		const std::string &desc,
		const std::string &val,
		const std::string &type );
	void AddKeyVal( const std::string &name, 
		const std::string &desc, 
		const std::string &val, 
		paramtypes_e type );
	void AddInput( const std::string &name, const std::string &desc );
	void AddInput( const CEntityRegisterInput &input );
	void AddOutput( const std::string &name, const std::string &desc );
	void AddOutput( const CEntityRegisterOutput &output );
	void Set( const std::string &name, const std::string &val );
	void Set( const CString &name, const CString &val );
	void Set( const char *name, const char *val );
	void Set( int i, const std::string &val );
	void Set( int i, const CString &val );

	const std::string &GetInternalType(void) { return m_internalType; }
private:
	std::string m_classname;
	std::string m_describtion;
	std::string m_internalType;
	std::map<std::string, std::string, LessThanCppStringObj> m_baseVals;
	std::vector<CEntityKeyValue*> m_KeyVals;
	std::vector<CEntityRegisterOutput> m_Outputs;
	std::vector<CEntityRegisterInput> m_Inputs;
};


typedef std::map<std::string, CEntityRegisterEntity*> EntityRegisterMap;
class CScriptSubGroup;

class CEntityRegister
{
public:
	void Init( void );
	CEntityRegisterEntity *GetEntity( const std::string &name );
	CEntityRegisterEntity *GetEntity( const CString &name );
	void ParseEntity( CEntityRegisterEntity *pEnt, CScriptSubGroup *pGrp );
	void GetEntityNames( std::vector<std::string> &vec );
	void ResolveBaseEntities( CEntityRegisterEntity *pEnt, std::string bases );
private:
	CScriptParser m_parser;
	EntityRegisterMap m_EntMap;
};


CEntityRegister *GetEntityRegister( void );

#endif // deferred__editor__CEntityRegister_H__
