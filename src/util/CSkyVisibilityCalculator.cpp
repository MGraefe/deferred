// util/CSkyVisibilityCalculator.cpp
// util/CSkyVisibilityCalculator.cpp
// util/CSkyVisibilityCalculator.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CSkyVisibilityCalculator.h"
#include "collision.h"
#include "CThread.h"
#include "COctreeNode.h"
#include "timer.h"

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::map;

const size_t rayCount = 128;
const CSkyVisibilityCalculator::ftype traceLength = 100000000.0;


//------------------------------------------------------------------------
// Purpose: Update progress display
//------------------------------------------------------------------------
class CPercentWriter
{
public:
		CPercentWriter(int chunkCount) :
			m_chunkCount(chunkCount),
			m_lastChunks(0) { }

		void start() {
			cout << "[";
			for( int i = 0; i < m_chunkCount; i++ )
				cout << "-";
			cout << "]";
			for( int i = 0; i < m_chunkCount+1; i++ )
				cout << "\b";
		}

		void update(real percent) {
			int chunks = (int)(percent * (real)m_chunkCount + 0.5);
			if( chunks > m_lastChunks )
			{
				for( int i = 0; i < chunks-m_lastChunks; i++ )
					cout << "*";
				m_lastChunks = chunks;
			}
		}

		void end() {
			update(1.0);
			cout << "]" << std::endl;
		}

private:
	int m_chunkCount;
	int m_lastChunks;
};


//------------------------------------------------------------------------
// Purpose: Worker thread
//------------------------------------------------------------------------
template<typename T>
class CVisibilityThread : public CThread
{
public:
	CVisibilityThread(const string &name, size_t start, size_t end,
		vector<Triangle_t<T>> *triangles, const CPlane_t<T> *planes,
		COctreeNode *octree, volatile T *percentVal) : 
		CThread(name),
		m_start(start),
		m_end(end),
		m_triangles(triangles),
		m_planes(planes),
		m_percentVal(percentVal),
		m_octree(octree),
		m_bCastVertices(false)
	{

	}

protected:
	virtual void ThreadRun(void);

private:
	size_t m_start;
	size_t m_end;
	volatile T *m_percentVal;
	vector<Triangle_t<T>> *m_triangles;
	const CPlane_t<T> *m_planes;
	COctreeNode *m_octree;
	const bool m_bCastVertices;
};


//------------------------------------------------------------------------
// Purpose: Generates random float in range [0.0, 1.0]
//------------------------------------------------------------------------
template<typename T>
inline T randomFloat(void)
{
	return (T)rand() * (T(1.0) / T(RAND_MAX));
}


//------------------------------------------------------------------------
// Purpose: Calculate the actual sky visibility factor from the number of
//			rays that hit geometry and the total number of rays
//------------------------------------------------------------------------
template<typename T>
T calcSkyVisibility(T hitRays, T rayCount)
{
	return T(1.0) - hitRays/rayCount;
}


//------------------------------------------------------------------------
// Purpose: Calculate sky visibility factor for a triangle, probe rays
//			look in the direction of the normal (hence the tangent and bitangent)
//------------------------------------------------------------------------
template<typename T>
T calcSkyVisibility(const COctreeNode *octree, const Vector3<T> &pos, const Vector3<T> &normal, const Vector3<T> &tangent, const Vector3<T> &bitangent,
	size_t rayCount)
{
	size_t hitRays = 0;
	for(size_t i = 0; i < rayCount; ++i)
	{
		Vector3<T> rnd(randomFloat<T>(), randomFloat<T>()*T(2.0)-T(1.0), randomFloat<T>()*T(2.0)-T(1.0));
		rnd.Normalize();
		Vector3<T> raydir = normal * rnd.x + tangent * rnd.y + bitangent * rnd.z;
		CRay_t<T> ray(pos + raydir * T(0.01), raydir.Normalize());
		if(octree->rayTriangleCollision(ray))
			hitRays++;
	}
	return calcSkyVisibility(T(hitRays), T(rayCount));
}


//------------------------------------------------------------------------
// Purpose: Calculate sky visibility factor for a lightprobe (single point),
//			probe rays look in to any direction
//------------------------------------------------------------------------
template<typename T>
T calcSkyVisibility(const COctreeNode *octree, const Vector3<T> &pos, size_t rayCount)
{
	size_t hitRays = 0;
	for(size_t i = 0; i < rayCount; i++)
	{
		Vector3<T> raydir(randomFloat<T>(), randomFloat<T>()*T(2.0)-T(1.0), randomFloat<T>()*T(2.0)-T(1.0));
		CRay_t<T> ray(pos, raydir.GetNormalized());
		if(octree->rayTriangleCollision(ray))
			hitRays++;
	}
	return calcSkyVisibility(T(hitRays), T(rayCount));
}


//------------------------------------------------------------------------
// Purpose: Thread run method, iterate over my triangle range and calculate
//			the sky visibility for my triangles
//------------------------------------------------------------------------
template<typename T>
void CVisibilityThread<T>::ThreadRun(void)
{
	vector<Triangle_t<T>> &triangles = *m_triangles;

	for( size_t i = m_start; i < m_end; ++i ) //triangle loop
	{
		Triangle_t<T> &tri = triangles[i];

		//Calculate perpendicular vectors
		Vector3<T> v1 = tri.norm3.GetPerpendicularVector().Normalize();
		Vector3<T> v2 = tri.norm3.Cross(v1);

		if( m_bCastVertices ) //Cast from triangle-vertices instead of center
		{
			T skyVisibilitySum = 0.0;
			for(size_t j = 0; j < 3; j++) //vertex loop
			{
				T skyVisibility = calcSkyVisibility(m_octree, tri.vertex3[j].pos3, tri.norm3, v1, v2, rayCount);
				tri.vertex3[j].skyVisibility = skyVisibility;
				skyVisibilitySum += skyVisibility;
			}
			tri.skyVisibility = skyVisibilitySum * (1.0/3.0);
		}
		else //cast from triangle-center instead of vertices
		{
			Vector3<T> triangleCenter = (tri.vertex3[0].pos3 + tri.vertex3[1].pos3 + tri.vertex3[2].pos3) / 3.0;
			tri.skyVisibility = calcSkyVisibility(m_octree, triangleCenter, tri.norm3, v1, v2, rayCount);
			tri.vertex3[0].skyVisibility = tri.skyVisibility;
			tri.vertex3[1].skyVisibility = tri.skyVisibility;
			tri.vertex3[2].skyVisibility = tri.skyVisibility;
		}

		*m_percentVal = (T)(i-m_start) / (T)(m_end-m_start);
	}
}

//------------------------------------------------------------------------
// Purpose: Sort vertexes first by x, then by y and then by z coordinate,
//			returning false if vertexes are equal regarding to a small offset
//			named epsilon
//------------------------------------------------------------------------
template<typename T>
class CWeldSkyVisComparatorLess
{
public:
	CWeldSkyVisComparatorLess(T epsilon, T normalEpsilon) : 
		eps(epsilon), normEps(normalEpsilon) {}
	bool operator() (	const Vertex_t<T> &left,
						const Vertex_t<T> &right ) const
	{
		if( abs(left.pos3.x - right.pos3.x) > eps )
			return left.pos3.x < right.pos3.x;
		if( abs(left.pos3.y - right.pos3.y) > eps )
			return left.pos3.y < right.pos3.y;
		if( abs(left.pos3.z - right.pos3.z) > eps )
			return left.pos3.z < right.pos3.z;
		return false;
	}

private:
	const real eps;
	const real normEps;
};


//------------------------------------------------------------------------
// Purpose: Naive, greedy cluster algorithm. Clusters objects in 'container'
//			into the container-container 'clusters' based on the comparison
//			operator 'op'
//------------------------------------------------------------------------
template<typename ContainerT, typename ClusterT, typename CompareOp>
void greedyCluster(const ContainerT &container, ClusterT &clusters, CompareOp op)
{
	for(typename ContainerT::const_reference newElement : container)
	{
		bool found = false;
		for(typename ClusterT::reference cluster : clusters)
		{
			typename ClusterT::value_type::const_reference first = *cluster.begin();
			if(op(first, newElement))
			{
				cluster.push_back(newElement);
				found = true;
				break;
			}
		}
		if(!found)
			clusters.push_back(typename ClusterT::value_type(1, newElement));
	}
}



//------------------------------------------------------------------------
// Purpose: Smooth sky visibility factors between adjacent triangles
//------------------------------------------------------------------------
void CSkyVisibilityCalculator::weldSkyVisibility(vector<Triangle> &triangles)
{
	typedef map<Vertex_t<ftype>, vector<Vertex_t<ftype>*>, CWeldSkyVisComparatorLess<ftype>> VertexMap;

	//Collect vertices
	ftype cosMaxAng = cos(0.5); //acos(0.5) = 60 degrees
	VertexMap vertexMap(CWeldSkyVisComparatorLess<ftype>(0.5, cosMaxAng)); 
	for( size_t i = 0; i < triangles.size(); ++i )
		for( size_t k = 0; k < 3; ++k )
			vertexMap[triangles[i].vertex3[k]].push_back(&triangles[i].vertex3[k]);

	//calculate average sky visibility of all vertexes in the same position and
	//store it
	size_t weldings = 0;
	VertexMap::const_iterator it = vertexMap.begin();
	VertexMap::const_iterator itEnd = vertexMap.end();
	for( ; it != itEnd; ++it )
	{
		const vector<Vertex_t<ftype>*> &verts = it->second;
		vector<vector<Vertex_t<ftype>*>> clusters; //Normal clusters
		greedyCluster(verts, clusters, [&](Vertex_t<ftype> *a, Vertex_t<ftype> *b){return a->norm3.Dot(b->norm3) > cosMaxAng;});
		for(vector<Vertex_t<ftype>*> &cluster : clusters)
		{
			ftype sum = 0.0;
			for( Vertex_t<ftype> *v : cluster )
				sum += v->skyVisibility;
			sum /= (ftype)cluster.size();
			for( Vertex_t<ftype> *v : cluster )
				v->skyVisibility = sum;
			weldings++;
		}
	}

	cout << "\t\twelded " << triangles.size()*3 << " to " << weldings << ", with " << vertexMap.size() << " initial clusters." << endl;
}


//------------------------------------------------------------------------
// Purpose: Constructor
//------------------------------------------------------------------------
CSkyVisibilityCalculator::CSkyVisibilityCalculator(std::vector<Triangle> &triangles, const AABoundingBox_t<real> &mapBounds) :
	m_triangles(triangles),
	m_mapBounds(mapBounds),
	m_planes(NULL),
	m_octree(NULL)
{

}


//---------------------------------------------------------------
// Purpose: Destructor
//---------------------------------------------------------------
CSkyVisibilityCalculator::~CSkyVisibilityCalculator()
{
	if(m_planes)
		delete[] m_planes;
	if(m_octree)
		delete m_octree;
}


//------------------------------------------------------------------------
// Purpose: Start sky visibility calculation
//------------------------------------------------------------------------
void CSkyVisibilityCalculator::calculate()
{
	cout << "Calculating sky-visibility..." << endl;
	gpGlobals_t globals;
	CCTime timer(&globals);
	timer.InitTime();
	UINT timeStart = timer.GetTimeMs();

	size_t numCores = std::thread::hardware_concurrency();
	size_t numThreads = max(m_triangles.size() / 700, 1u);

	calcOctree();

	volatile ftype *percentVals = new ftype[numThreads];
	for( size_t i = 0; i < numThreads; i++ )
		percentVals[i] = ftype(0.0);

	size_t iStart = 0;
	size_t iMax = m_triangles.size();
	CVisibilityThread<ftype>** threads = new CVisibilityThread<ftype>*[numThreads];

	cout << "\tLaunching " << numThreads << " threads with a maximum of " << numCores << " active at the same time." << endl;

	//Init Threads
	for( size_t i = 0; i < numThreads; i++ )
	{
		size_t iEnd = i == numThreads-1 ? iMax : (i+1)*(iMax/numThreads);
		char threadName[64];
		sprintf_s(threadName, "SkyVisibilityThread %i of %i", i+1, numThreads);
		threads[i] = new CVisibilityThread<ftype>(string(threadName), iStart, iEnd, &m_triangles, m_planes, m_octree, &percentVals[i]);
		iStart = iEnd;
	}

	//Init the percent writer and wait for threads to complete
	CPercentWriter writer(75);
	writer.start();
	size_t queueIndex = 0;
	while(true)
	{
		CCTime::Sleep(20);
		ftype percent = ftype(0.0);
		for( size_t i = 0; i < numThreads; i++ )
			percent += percentVals[i];
		percent /= (ftype)numThreads;
		writer.update(percent);

		size_t runningThreads = 0;
		for( size_t i = 0; i < numThreads; i++ )
			if( threads[i]->IsRunning() )
				runningThreads++;
		if( runningThreads < numCores && queueIndex < numThreads )
			threads[queueIndex++]->Start();
		if( runningThreads == 0 && queueIndex == numThreads )
			break;
	}
	writer.end();

	ftype avgSkyVisibility = ftype(0.0);
	for( size_t i = 0; i < m_triangles.size(); i++ )
		avgSkyVisibility += m_triangles[i].skyVisibility;
	avgSkyVisibility /= ftype(m_triangles.size());
	cout << "\tAverage sky-visibility: " << avgSkyVisibility << "" << endl;

	cout << "\tWelding sky-visibility..." << endl;
	weldSkyVisibility(m_triangles);
	cout << "\tdone." << endl;

	float time = (float)(timer.GetTimeMs()-timeStart)/1000.0f;
	cout << "done. (" << time << "s)\n" << endl;

	for( size_t i = 0; i < numThreads; i++ )
		delete threads[i];
	delete[] threads;
	delete[] percentVals;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSkyVisibilityCalculator::calcOctree()
{
	if(!m_octree)
	{
		//Create planes
		Assert(!m_planes);
		CPlane_t<ftype> *m_planes = new CPlane_t<ftype>[m_triangles.size()];
		std::vector<int> startIndices(m_triangles.size());
		for( size_t i = 0; i < m_triangles.size(); ++i )
		{
			startIndices[i] = i;
			m_planes[i].redefine(m_triangles[i].vertex3[0].pos3, m_triangles[i].norm3);
		}

		//Create octree-structure
		cout << "\tBuilding Octree to accelerate raytracing" << endl;
		m_octree = new COctreeNode(&m_triangles, m_planes, startIndices, m_mapBounds, 10, 0, 16);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSkyVisibilityCalculator::calculateLightProbeSkyVis( std::vector<CLightProbe> &probes )
{
	calcOctree();
	for(CLightProbe &probe : probes)
		probe.SetLight((float)calcSkyVisibility(m_octree, probe.GetPosition().toVec3d(), 512));
}
