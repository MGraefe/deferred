// util/rpm_file.h
// util/rpm_file.h
// util/rpm_file.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#ifndef rpm_file
#define rpm_file

#include <cstdint>
#include <istream>
#include <ostream>
#include <memory>

enum rpm_lumps_e
{
	LUMP_TRIANGLES = 0,
	LUMP_SKYVIS,
	LUMP_TEXOFS,
	LUMP_TEXNAMES,
	LUMP_LIGHTPROBES,

	LUMP_COUNT__, //has to be the last
};

typedef short int SHORT;

struct rpm_vertex_t
{
	Vector3f pos3;
	Vector3f norm3;
	Vector2f uv2;
};

struct rpm_triangle_t
{
	rpm_vertex_t vertex3[3];
	Vector3f	 norm3;
	int			 iTexID;
};


#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct rpm_header_t
{
	rpm_header_t() {
		version = 0;
		num_of_triangles = 0;
		sTexOfsetts = 0;
		iTexdataLength = 0;
	}

	SHORT			version;
	SHORT			unused;
	UINT			num_of_triangles;
	AABoundingBox	bounds;
	UINT			sTexOfsetts;
	UINT			iTexdataLength;
};
#pragma pack(pop) /* restore original alignment from stack */

class rpm_lump
{
public:
	rpm_lump();
	rpm_lump(UINT ident);
	rpm_lump(UINT ident, UINT size, char *data);
	//rpm_lump(const rpm_lump &other);
	~rpm_lump();
	bool serializeHeader(std::ostream &os, UINT &dataStart);
	bool serializeData(std::ostream &os);
	bool deserialize(std::istream &is);
	inline static UINT headerSize() {
		return sizeof(UINT) + sizeof(UINT) + sizeof(UINT);
	}
	void alloc(UINT size);
	char *getData() const;
	inline UINT getSize() const { return m_size; }
	inline UINT getIdent() const { return m_ident; }

private:
	UINT m_ident;
	UINT m_size;
	std::shared_ptr<char> m_data;
	UINT m_start;
};


class rpm_lumps
{
public:
	rpm_lumps();
	~rpm_lumps();
	bool serialize(std::ostream &os);
	bool deserialize(std::istream &is);
	rpm_lump *findLump(rpm_lumps_e lump) const;
	rpm_lump *addLump(const rpm_lump &lump);
	void clear();
private:
	std::vector<rpm_lump*> lumps;
};


class rpm_file_t
{
public:
	enum rpm_load_errors_e
	{
		RPM_LOAD_OK = 0,
		RPM_LOAD_FILENOTFOUND,
		RPM_LOAD_WRONGVERSION,
		RPM_LOAD_CORRUPTFILE,
	};

	enum rpm_write_errors_e
	{
		RPM_WRITE_OK = 0,
		RPM_WRITE_ERROR = 1,
	};

	rpm_file_t();
	rpm_file_t(const rpm_file_t &other);
	~rpm_file_t();
	rpm_file_t &operator=(const rpm_file_t &other);
	const char *getTexName( int texID ) const;
	const char *getTexName( const rpm_triangle_t &triangle ) const;
	int getTriangleCount( void ) const;
	int getTextureCount( void ) const;
	int getTexDataLength( void ) const;
	int getVersion( void ) const;
	float getSkyVisibility(int triangle, int vertex) const;
	const AABoundingBox &getBounds( void ) const;
	rpm_lump *findLump(rpm_lumps_e lump) const;
	rpm_lump *createLump(rpm_lumps_e ident);
	rpm_lump *addLump(const rpm_lump &lump);
	rpm_load_errors_e LoadFromFile( const char *filename );
	rpm_load_errors_e LoadFromFile( const wchar_t *filename );
	rpm_load_errors_e Deserialize( std::istream &is );
	rpm_write_errors_e Serialize( std::ostream &os );
	
	//Only valid after deserialization
	inline const rpm_triangle_t *getTriangles() const { return triangles; }

	//Only valid after deserialization
	inline const float *getSkyVisibility() const { return skyVisibilities; }

	//Only valid after deserialization
	inline const int *getTexofs() const { return texofs; }

	//Only valid after deserialization
	inline const char *getTexnames() const { return texnames; }
	void clearHeap();
	rpm_header_t	header;

private:
	
	rpm_file_t(rpm_file_t &&other) {}

	//These are the convenience lumps.
	//When deserializing, these pointers get initialized to the lump-data (if lump is present)
	//so that we have easy and fast access to the data.
	rpm_triangle_t	*triangles;
	float			*skyVisibilities; //per vertex
	int				*texofs;
	char			*texnames;

	rpm_lumps		lumps;
};


#endif
