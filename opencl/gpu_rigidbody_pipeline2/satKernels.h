//this file is autogenerated using stringify.bat (premake --stringify) in the build folder of this project
static const char* satKernelsCL= \
"\n"
"//keep this enum in sync with the CPU version (in AdlCollisionShape.h)\n"
"#define SHAPE_CONVEX_HULL 3\n"
"#define SHAPE_CONCAVE_TRIMESH 5\n"
"#define TRIANGLE_NUM_CONVEX_FACES 5\n"
"\n"
"typedef unsigned int u32;\n"
"\n"
"///keep this in sync with btCollidable.h\n"
"typedef struct\n"
"{\n"
"	int m_shapeType;\n"
"	int m_shapeIndex;\n"
"	\n"
"} btCollidableGpu;\n"
"\n"
"typedef struct\n"
"{\n"
"	float4 m_pos;\n"
"	float4 m_quat;\n"
"	float4 m_linVel;\n"
"	float4 m_angVel;\n"
"\n"
"	u32 m_collidableIdx;\n"
"	float m_invMass;\n"
"	float m_restituitionCoeff;\n"
"	float m_frictionCoeff;\n"
"} BodyData;\n"
"\n"
"\n"
"typedef struct  \n"
"{\n"
"	float4		m_localCenter;\n"
"	float4		m_extents;\n"
"	float4		mC;\n"
"	float4		mE;\n"
"	\n"
"	float			m_radius;\n"
"	int	m_faceOffset;\n"
"	int m_numFaces;\n"
"	int	m_numVertices;\n"
"\n"
"	int m_vertexOffset;\n"
"	int	m_uniqueEdgesOffset;\n"
"	int	m_numUniqueEdges;\n"
"	int m_unused;\n"
"} ConvexPolyhedronCL;\n"
"\n"
"typedef struct \n"
"{\n"
"	union\n"
"	{\n"
"		float4	m_min;\n"
"		float   m_minElems[4];\n"
"		int			m_minIndices[4];\n"
"	};\n"
"	union\n"
"	{\n"
"		float4	m_max;\n"
"		float   m_maxElems[4];\n"
"		int			m_maxIndices[4];\n"
"	};\n"
"} btAabbCL;\n"
"\n"
"typedef struct\n"
"{\n"
"	float4 m_plane;\n"
"	int m_indexOffset;\n"
"	int m_numIndices;\n"
"} btGpuFace;\n"
"\n"
"#define make_float4 (float4)\n"
"\n"
"__inline\n"
"float4 cross3(float4 a, float4 b)\n"
"{\n"
"	return cross(a,b);\n"
"}\n"
"\n"
"__inline\n"
"float dot3F4(float4 a, float4 b)\n"
"{\n"
"	float4 a1 = make_float4(a.xyz,0.f);\n"
"	float4 b1 = make_float4(b.xyz,0.f);\n"
"	return dot(a1, b1);\n"
"}\n"
"\n"
"__inline\n"
"float4 fastNormalize4(float4 v)\n"
"{\n"
"	return fast_normalize(v);\n"
"}\n"
"\n"
"\n"
"///////////////////////////////////////\n"
"//	Quaternion\n"
"///////////////////////////////////////\n"
"\n"
"typedef float4 Quaternion;\n"
"\n"
"__inline\n"
"Quaternion qtMul(Quaternion a, Quaternion b);\n"
"\n"
"__inline\n"
"Quaternion qtNormalize(Quaternion in);\n"
"\n"
"__inline\n"
"float4 qtRotate(Quaternion q, float4 vec);\n"
"\n"
"__inline\n"
"Quaternion qtInvert(Quaternion q);\n"
"\n"
"\n"
"\n"
"\n"
"__inline\n"
"Quaternion qtMul(Quaternion a, Quaternion b)\n"
"{\n"
"	Quaternion ans;\n"
"	ans = cross3( a, b );\n"
"	ans += a.w*b+b.w*a;\n"
"//	ans.w = a.w*b.w - (a.x*b.x+a.y*b.y+a.z*b.z);\n"
"	ans.w = a.w*b.w - dot3F4(a, b);\n"
"	return ans;\n"
"}\n"
"\n"
"__inline\n"
"Quaternion qtNormalize(Quaternion in)\n"
"{\n"
"	return fastNormalize4(in);\n"
"//	in /= length( in );\n"
"//	return in;\n"
"}\n"
"__inline\n"
"float4 qtRotate(Quaternion q, float4 vec)\n"
"{\n"
"	Quaternion qInv = qtInvert( q );\n"
"	float4 vcpy = vec;\n"
"	vcpy.w = 0.f;\n"
"	float4 out = qtMul(qtMul(q,vcpy),qInv);\n"
"	return out;\n"
"}\n"
"\n"
"__inline\n"
"Quaternion qtInvert(Quaternion q)\n"
"{\n"
"	return (Quaternion)(-q.xyz, q.w);\n"
"}\n"
"\n"
"__inline\n"
"float4 qtInvRotate(const Quaternion q, float4 vec)\n"
"{\n"
"	return qtRotate( qtInvert( q ), vec );\n"
"}\n"
"\n"
"__inline\n"
"float4 transform(const float4* p, const float4* translation, const Quaternion* orientation)\n"
"{\n"
"	return qtRotate( *orientation, *p ) + (*translation);\n"
"}\n"
"\n"
"\n"
"\n"
"__inline\n"
"float4 normalize3(const float4 a)\n"
"{\n"
"	float4 n = make_float4(a.x, a.y, a.z, 0.f);\n"
"	return fastNormalize4( n );\n"
"}\n"
"\n"
"inline void projectLocal(const ConvexPolyhedronCL* hull,  const float4 pos, const float4 orn, \n"
"const float4* dir, const float4* vertices, float* min, float* max)\n"
"{\n"
"	min[0] = FLT_MAX;\n"
"	max[0] = -FLT_MAX;\n"
"	int numVerts = hull->m_numVertices;\n"
"\n"
"	const float4 localDir = qtInvRotate(orn,*dir);\n"
"	float offset = dot(pos,*dir);\n"
"	for(int i=0;i<numVerts;i++)\n"
"	{\n"
"		float dp = dot(vertices[hull->m_vertexOffset+i],localDir);\n"
"		if(dp < min[0])	\n"
"			min[0] = dp;\n"
"		if(dp > max[0])	\n"
"			max[0] = dp;\n"
"	}\n"
"	if(min[0]>max[0])\n"
"	{\n"
"		float tmp = min[0];\n"
"		min[0] = max[0];\n"
"		max[0] = tmp;\n"
"	}\n"
"	min[0] += offset;\n"
"	max[0] += offset;\n"
"}\n"
"\n"
"inline void project(__global const ConvexPolyhedronCL* hull,  const float4 pos, const float4 orn, \n"
"const float4* dir, __global const float4* vertices, float* min, float* max)\n"
"{\n"
"	min[0] = FLT_MAX;\n"
"	max[0] = -FLT_MAX;\n"
"	int numVerts = hull->m_numVertices;\n"
"\n"
"	const float4 localDir = qtInvRotate(orn,*dir);\n"
"	float offset = dot(pos,*dir);\n"
"	for(int i=0;i<numVerts;i++)\n"
"	{\n"
"		float dp = dot(vertices[hull->m_vertexOffset+i],localDir);\n"
"		if(dp < min[0])	\n"
"			min[0] = dp;\n"
"		if(dp > max[0])	\n"
"			max[0] = dp;\n"
"	}\n"
"	if(min[0]>max[0])\n"
"	{\n"
"		float tmp = min[0];\n"
"		min[0] = max[0];\n"
"		max[0] = tmp;\n"
"	}\n"
"	min[0] += offset;\n"
"	max[0] += offset;\n"
"}\n"
"\n"
"inline bool TestSepAxisLocalA(const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA,const float4 ornA,\n"
"	const float4 posB,const float4 ornB,\n"
"	float4* sep_axis, const float4* verticesA, __global const float4* verticesB,float* depth)\n"
"{\n"
"	float Min0,Max0;\n"
"	float Min1,Max1;\n"
"	projectLocal(hullA,posA,ornA,sep_axis,verticesA, &Min0, &Max0);\n"
"	project(hullB,posB,ornB, sep_axis,verticesB, &Min1, &Max1);\n"
"\n"
"	if(Max0<Min1 || Max1<Min0)\n"
"		return false;\n"
"\n"
"	float d0 = Max0 - Min1;\n"
"	float d1 = Max1 - Min0;\n"
"	*depth = d0<d1 ? d0:d1;\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"inline bool IsAlmostZero(const float4 v)\n"
"{\n"
"	if(fabs(v.x)>1e-6f || fabs(v.y)>1e-6f || fabs(v.z)>1e-6f)	\n"
"		return false;\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"\n"
"bool findSeparatingAxisLocalA(	const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA1,\n"
"	const float4 ornA,\n"
"	const float4 posB1,\n"
"	const float4 ornB,\n"
"	const float4 DeltaC2,\n"
"	\n"
"	const float4* verticesA, \n"
"	const float4* uniqueEdgesA, \n"
"	const btGpuFace* facesA,\n"
"	const int*  indicesA,\n"
"\n"
"	__global const float4* verticesB, \n"
"	__global const float4* uniqueEdgesB, \n"
"	__global const btGpuFace* facesB,\n"
"	__global const int*  indicesB,\n"
"	float4* sep,\n"
"	float* dmin)\n"
"{\n"
"	int i = get_global_id(0);\n"
"\n"
"	float4 posA = posA1;\n"
"	posA.w = 0.f;\n"
"	float4 posB = posB1;\n"
"	posB.w = 0.f;\n"
"	int curPlaneTests=0;\n"
"	{\n"
"		int numFacesA = hullA->m_numFaces;\n"
"		// Test normals from hullA\n"
"		for(int i=0;i<numFacesA;i++)\n"
"		{\n"
"			const float4 normal = facesA[hullA->m_faceOffset+i].m_plane;\n"
"			float4 faceANormalWS = qtRotate(ornA,normal);\n"
"			if (dot3F4(DeltaC2,faceANormalWS)<0)\n"
"				faceANormalWS*=-1.f;\n"
"			curPlaneTests++;\n"
"			float d;\n"
"			if(!TestSepAxisLocalA( hullA, hullB, posA,ornA,posB,ornB,&faceANormalWS, verticesA, verticesB,&d))\n"
"				return false;\n"
"			if(d<*dmin)\n"
"			{\n"
"				*dmin = d;\n"
"				*sep = faceANormalWS;\n"
"			}\n"
"		}\n"
"	}\n"
"	if((dot3F4(-DeltaC2,*sep))>0.0f)\n"
"	{\n"
"		*sep = -(*sep);\n"
"	}\n"
"	return true;\n"
"}\n"
"\n"
"bool findSeparatingAxisLocalB(	__global const ConvexPolyhedronCL* hullA,  const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA1,\n"
"	const float4 ornA,\n"
"	const float4 posB1,\n"
"	const float4 ornB,\n"
"	const float4 DeltaC2,\n"
"	__global const float4* verticesA, \n"
"	__global const float4* uniqueEdgesA, \n"
"	__global const btGpuFace* facesA,\n"
"	__global const int*  indicesA,\n"
"	const float4* verticesB,\n"
"	const float4* uniqueEdgesB, \n"
"	const btGpuFace* facesB,\n"
"	const int*  indicesB,\n"
"	float4* sep,\n"
"	float* dmin)\n"
"{\n"
"	int i = get_global_id(0);\n"
"\n"
"	float4 posA = posA1;\n"
"	posA.w = 0.f;\n"
"	float4 posB = posB1;\n"
"	posB.w = 0.f;\n"
"	int curPlaneTests=0;\n"
"	{\n"
"		int numFacesA = hullA->m_numFaces;\n"
"		// Test normals from hullA\n"
"		for(int i=0;i<numFacesA;i++)\n"
"		{\n"
"			const float4 normal = facesA[hullA->m_faceOffset+i].m_plane;\n"
"			float4 faceANormalWS = qtRotate(ornA,normal);\n"
"			if (dot3F4(DeltaC2,faceANormalWS)<0)\n"
"				faceANormalWS *= -1.f;\n"
"			curPlaneTests++;\n"
"			float d;\n"
"			if(!TestSepAxisLocalA( hullB, hullA, posB,ornB,posA,ornA, &faceANormalWS, verticesB,verticesA, &d))\n"
"				return false;\n"
"			if(d<*dmin)\n"
"			{\n"
"				*dmin = d;\n"
"				*sep = faceANormalWS;\n"
"			}\n"
"		}\n"
"	}\n"
"	if((dot3F4(-DeltaC2,*sep))>0.0f)\n"
"	{\n"
"		*sep = -(*sep);\n"
"	}\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"\n"
"bool findSeparatingAxisEdgeEdgeLocalA(	const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA1,\n"
"	const float4 ornA,\n"
"	const float4 posB1,\n"
"	const float4 ornB,\n"
"	const float4 DeltaC2,\n"
"	const float4* verticesA, \n"
"	const float4* uniqueEdgesA, \n"
"	const btGpuFace* facesA,\n"
"	const int*  indicesA,\n"
"	__global const float4* verticesB, \n"
"	__global const float4* uniqueEdgesB, \n"
"	__global const btGpuFace* facesB,\n"
"	__global const int*  indicesB,\n"
"		float4* sep,\n"
"	float* dmin)\n"
"{\n"
"	int i = get_global_id(0);\n"
"\n"
"	float4 posA = posA1;\n"
"	posA.w = 0.f;\n"
"	float4 posB = posB1;\n"
"	posB.w = 0.f;\n"
"\n"
"	int curPlaneTests=0;\n"
"\n"
"	int curEdgeEdge = 0;\n"
"	// Test edges\n"
"	for(int e0=0;e0<hullA->m_numUniqueEdges;e0++)\n"
"	{\n"
"		const float4 edge0 = uniqueEdgesA[hullA->m_uniqueEdgesOffset+e0];\n"
"		float4 edge0World = qtRotate(ornA,edge0);\n"
"\n"
"		for(int e1=0;e1<hullB->m_numUniqueEdges;e1++)\n"
"		{\n"
"			const float4 edge1 = uniqueEdgesB[hullB->m_uniqueEdgesOffset+e1];\n"
"			float4 edge1World = qtRotate(ornB,edge1);\n"
"\n"
"\n"
"			float4 crossje = cross3(edge0World,edge1World);\n"
"\n"
"			curEdgeEdge++;\n"
"			if(!IsAlmostZero(crossje))\n"
"			{\n"
"				crossje = normalize3(crossje);\n"
"				if (dot3F4(DeltaC2,crossje)<0)\n"
"					crossje *= -1.f;\n"
"\n"
"				float dist;\n"
"				bool result = true;\n"
"				{\n"
"					float Min0,Max0;\n"
"					float Min1,Max1;\n"
"					projectLocal(hullA,posA,ornA,&crossje,verticesA, &Min0, &Max0);\n"
"					project(hullB,posB,ornB,&crossje,verticesB, &Min1, &Max1);\n"
"				\n"
"					if(Max0<Min1 || Max1<Min0)\n"
"						result = false;\n"
"				\n"
"					float d0 = Max0 - Min1;\n"
"					float d1 = Max1 - Min0;\n"
"					dist = d0<d1 ? d0:d1;\n"
"					result = true;\n"
"\n"
"				}\n"
"				\n"
"\n"
"				if(dist<*dmin)\n"
"				{\n"
"					*dmin = dist;\n"
"					*sep = crossje;\n"
"				}\n"
"			}\n"
"		}\n"
"\n"
"	}\n"
"\n"
"	\n"
"	if((dot3F4(-DeltaC2,*sep))>0.0f)\n"
"	{\n"
"		*sep = -(*sep);\n"
"	}\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"inline bool TestSepAxis(__global const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA,const float4 ornA,\n"
"	const float4 posB,const float4 ornB,\n"
"	float4* sep_axis, __global const float4* vertices,float* depth)\n"
"{\n"
"	float Min0,Max0;\n"
"	float Min1,Max1;\n"
"	project(hullA,posA,ornA,sep_axis,vertices, &Min0, &Max0);\n"
"	project(hullB,posB,ornB, sep_axis,vertices, &Min1, &Max1);\n"
"\n"
"	if(Max0<Min1 || Max1<Min0)\n"
"		return false;\n"
"\n"
"	float d0 = Max0 - Min1;\n"
"	float d1 = Max1 - Min0;\n"
"	*depth = d0<d1 ? d0:d1;\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"bool findSeparatingAxis(	__global const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA1,\n"
"	const float4 ornA,\n"
"	const float4 posB1,\n"
"	const float4 ornB,\n"
"	const float4 DeltaC2,\n"
"	__global const float4* vertices, \n"
"	__global const float4* uniqueEdges, \n"
"	__global const btGpuFace* faces,\n"
"	__global const int*  indices,\n"
"	__global volatile float4* sep,\n"
"	float* dmin)\n"
"{\n"
"	int i = get_global_id(0);\n"
"\n"
"	float4 posA = posA1;\n"
"	posA.w = 0.f;\n"
"	float4 posB = posB1;\n"
"	posB.w = 0.f;\n"
"	\n"
"	int curPlaneTests=0;\n"
"\n"
"	{\n"
"		int numFacesA = hullA->m_numFaces;\n"
"		// Test normals from hullA\n"
"		for(int i=0;i<numFacesA;i++)\n"
"		{\n"
"			const float4 normal = faces[hullA->m_faceOffset+i].m_plane;\n"
"			float4 faceANormalWS = qtRotate(ornA,normal);\n"
"	\n"
"			if (dot3F4(DeltaC2,faceANormalWS)<0)\n"
"				faceANormalWS*=-1.f;\n"
"				\n"
"			curPlaneTests++;\n"
"	\n"
"			float d;\n"
"			if(!TestSepAxis( hullA, hullB, posA,ornA,posB,ornB,&faceANormalWS, vertices,&d))\n"
"				return false;\n"
"	\n"
"			if(d<*dmin)\n"
"			{\n"
"				*dmin = d;\n"
"				*sep = faceANormalWS;\n"
"			}\n"
"		}\n"
"	}\n"
"\n"
"	\n"
"	if((dot3F4(-DeltaC2,*sep))>0.0f)\n"
"	{\n"
"		*sep = -(*sep);\n"
"	}\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"bool findSeparatingAxisEdgeEdge(	__global const ConvexPolyhedronCL* hullA, __global const ConvexPolyhedronCL* hullB, \n"
"	const float4 posA1,\n"
"	const float4 ornA,\n"
"	const float4 posB1,\n"
"	const float4 ornB,\n"
"	const float4 DeltaC2,\n"
"	__global const float4* vertices, \n"
"	__global const float4* uniqueEdges, \n"
"	__global const btGpuFace* faces,\n"
"	__global const int*  indices,\n"
"	__global volatile float4* sep,\n"
"	float* dmin)\n"
"{\n"
"	int i = get_global_id(0);\n"
"\n"
"	float4 posA = posA1;\n"
"	posA.w = 0.f;\n"
"	float4 posB = posB1;\n"
"	posB.w = 0.f;\n"
"\n"
"	int curPlaneTests=0;\n"
"\n"
"	int curEdgeEdge = 0;\n"
"	// Test edges\n"
"	for(int e0=0;e0<hullA->m_numUniqueEdges;e0++)\n"
"	{\n"
"		const float4 edge0 = uniqueEdges[hullA->m_uniqueEdgesOffset+e0];\n"
"		float4 edge0World = qtRotate(ornA,edge0);\n"
"\n"
"		for(int e1=0;e1<hullB->m_numUniqueEdges;e1++)\n"
"		{\n"
"			const float4 edge1 = uniqueEdges[hullB->m_uniqueEdgesOffset+e1];\n"
"			float4 edge1World = qtRotate(ornB,edge1);\n"
"\n"
"\n"
"			float4 crossje = cross3(edge0World,edge1World);\n"
"\n"
"			curEdgeEdge++;\n"
"			if(!IsAlmostZero(crossje))\n"
"			{\n"
"				crossje = normalize3(crossje);\n"
"				if (dot3F4(DeltaC2,crossje)<0)\n"
"					crossje*=-1.f;\n"
"					\n"
"				float dist;\n"
"				bool result = true;\n"
"				{\n"
"					float Min0,Max0;\n"
"					float Min1,Max1;\n"
"					project(hullA,posA,ornA,&crossje,vertices, &Min0, &Max0);\n"
"					project(hullB,posB,ornB,&crossje,vertices, &Min1, &Max1);\n"
"				\n"
"					if(Max0<Min1 || Max1<Min0)\n"
"						result = false;\n"
"				\n"
"					float d0 = Max0 - Min1;\n"
"					float d1 = Max1 - Min0;\n"
"					dist = d0<d1 ? d0:d1;\n"
"					result = true;\n"
"\n"
"				}\n"
"				\n"
"\n"
"				if(dist<*dmin)\n"
"				{\n"
"					*dmin = dist;\n"
"					*sep = crossje;\n"
"				}\n"
"			}\n"
"		}\n"
"\n"
"	}\n"
"\n"
"	\n"
"	if((dot3F4(-DeltaC2,*sep))>0.0f)\n"
"	{\n"
"		*sep = -(*sep);\n"
"	}\n"
"	return true;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"// work-in-progress\n"
"__kernel void   findSeparatingAxisKernel( __global const int2* pairs, \n"
"																					__global const BodyData* rigidBodies, \n"
"																					__global const btCollidableGpu* collidables,\n"
"																					__global const ConvexPolyhedronCL* convexShapes, \n"
"																					__global const float4* vertices,\n"
"																					__global const float4* uniqueEdges,\n"
"																					__global const btGpuFace* faces,\n"
"																					__global const int* indices,\n"
"																					__global btAabbCL* aabbs,\n"
"																					__global volatile float4* separatingNormals,\n"
"																					__global volatile int* hasSeparatingAxis,\n"
"																					__global int4* concavePairsOut,\n"
"																					__global float4* concaveSeparatingNormalsOut,\n"
"																					__global volatile int* numConcavePairsOut,\n"
"																					int numPairs,\n"
"																					int maxnumConcavePairsCapacity\n"
"																					)\n"
"{\n"
"\n"
"	int i = get_global_id(0);\n"
"	\n"
"	if (i<numPairs)\n"
"	{\n"
"		int bodyIndexA = pairs[i].x;\n"
"		int bodyIndexB = pairs[i].y;\n"
"\n"
"		int collidableIndexA = rigidBodies[bodyIndexA].m_collidableIdx;\n"
"		int collidableIndexB = rigidBodies[bodyIndexB].m_collidableIdx;\n"
"\n"
"		int shapeIndexA = collidables[collidableIndexA].m_shapeIndex;\n"
"		int shapeIndexB = collidables[collidableIndexB].m_shapeIndex;\n"
"		\n"
"		if ((collidables[collidableIndexA].m_shapeType==SHAPE_CONCAVE_TRIMESH))// && (collidables[collidableIndexB].m_shapeType==SHAPE_CONVEX_HULL))\n"
"		{\n"
"	\n"
"			int numFacesA = convexShapes[shapeIndexA].m_numFaces;\n"
"			int numActualConcaveConvexTests = 0;\n"
"			\n"
"			//for (int f=0;f<numFacesA;f++)\n"
"			for (int f=0;f<numFacesA;f+=2)\n"
"			{\n"
"			\n"
"				bool overlap = false;\n"
"				\n"
"				ConvexPolyhedronCL convexPolyhedronA;\n"
"\n"
"			//add 3 vertices of the triangle\n"
"				convexPolyhedronA.m_numVertices = 3;\n"
"				convexPolyhedronA.m_vertexOffset = 0;\n"
"				float4	localCenter = make_float4(0.f,0.f,0.f,0.f);\n"
"\n"
"				btGpuFace face = faces[convexShapes[shapeIndexA].m_faceOffset+f];\n"
"				float4 triMinAabb, triMaxAabb;\n"
"				btAabbCL triAabb;\n"
"				triAabb.m_min = make_float4(1e30f,1e30f,1e30f,0.f);\n"
"				triAabb.m_max = make_float4(-1e30f,-1e30f,-1e30f,0.f);\n"
"				\n"
"				float4 verticesA[3];\n"
"				for (int i=0;i<3;i++)\n"
"				{\n"
"					int index = indices[face.m_indexOffset+i];\n"
"					float4 vert = vertices[convexShapes[shapeIndexA].m_vertexOffset+index];\n"
"					verticesA[i] = vert;\n"
"					localCenter += vert;\n"
"#if 0\n"
"//just in case some implementation doesn't support component-wise min and max for float4\n"
"					if (triAabb.m_min.x > vert.x)\n"
"						triAabb.m_min.x = vert.x;\n"
"					if (triAabb.m_min.y > vert.y)\n"
"						triAabb.m_min.y = vert.y;\n"
"					if (triAabb.m_min.z > vert.z)\n"
"						triAabb.m_min.z = vert.z;\n"
"\n"
"					if (triAabb.m_max.x < vert.x)\n"
"						triAabb.m_max.x = vert.x;\n"
"					if (triAabb.m_max.y < vert.y)\n"
"						triAabb.m_max.y = vert.y;\n"
"					if (triAabb.m_max.z < vert.z)\n"
"						triAabb.m_max.z = vert.z;\n"
"#else				\n"
"					triAabb.m_min = min(triAabb.m_min,vert);		\n"
"					triAabb.m_max = max(triAabb.m_max,vert);		\n"
"#endif					\n"
"				}\n"
"\n"
"				overlap = true;\n"
"				overlap = (triAabb.m_min.x > aabbs[bodyIndexB].m_max.x || triAabb.m_max.x < aabbs[bodyIndexB].m_min.x) ? false : overlap;\n"
"				overlap = (triAabb.m_min.z > aabbs[bodyIndexB].m_max.z || triAabb.m_max.z < aabbs[bodyIndexB].m_min.z) ? false : overlap;\n"
"				overlap = (triAabb.m_min.y > aabbs[bodyIndexB].m_max.y || triAabb.m_max.y < aabbs[bodyIndexB].m_min.y) ? false : overlap;\n"
"					\n"
"				if (overlap)\n"
"				{\n"
"					float dmin = FLT_MAX;\n"
"					int hasSeparatingAxis=5;\n"
"					float4 sepAxis=make_float4(1,2,3,4);\n"
"\n"
"#if 1\n"
"					\n"
"					int localCC=0;\n"
"					numActualConcaveConvexTests++;\n"
"\n"
"					//a triangle has 3 unique edges\n"
"					convexPolyhedronA.m_numUniqueEdges = 3;\n"
"					convexPolyhedronA.m_uniqueEdgesOffset = 0;\n"
"					float4 uniqueEdgesA[3];\n"
"					\n"
"					uniqueEdgesA[0] = (verticesA[1]-verticesA[0]);\n"
"					uniqueEdgesA[1] = (verticesA[2]-verticesA[1]);\n"
"					uniqueEdgesA[2] = (verticesA[0]-verticesA[2]);\n"
"\n"
"\n"
"					convexPolyhedronA.m_faceOffset = 0;\n"
"                                        \n"
"					float4 normal = make_float4(face.m_plane.x,face.m_plane.y,face.m_plane.z,0.f);\n"
"                                   \n"
"					btGpuFace facesA[TRIANGLE_NUM_CONVEX_FACES];\n"
"					int indicesA[3+3+2+2+2];\n"
"					int curUsedIndices=0;\n"
"					int fidx=0;\n"
"\n"
"					//front size of triangle\n"
"					{\n"
"						facesA[fidx].m_indexOffset=curUsedIndices;\n"
"						indicesA[0] = 0;\n"
"						indicesA[1] = 1;\n"
"						indicesA[2] = 2;\n"
"						curUsedIndices+=3;\n"
"						float c = face.m_plane.w;\n"
"						facesA[fidx].m_plane.x = normal.x;\n"
"						facesA[fidx].m_plane.y = normal.y;\n"
"						facesA[fidx].m_plane.z = normal.z;\n"
"						facesA[fidx].m_plane.w = c;\n"
"						facesA[fidx].m_numIndices=3;\n"
"					}\n"
"					fidx++;\n"
"					//back size of triangle\n"
"					{\n"
"						facesA[fidx].m_indexOffset=curUsedIndices;\n"
"						indicesA[3]=2;\n"
"						indicesA[4]=1;\n"
"						indicesA[5]=0;\n"
"						curUsedIndices+=3;\n"
"						float c = dot(normal,verticesA[0]);\n"
"						float c1 = -face.m_plane.w;\n"
"						facesA[fidx].m_plane.x = -normal.x;\n"
"						facesA[fidx].m_plane.y = -normal.y;\n"
"						facesA[fidx].m_plane.z = -normal.z;\n"
"						facesA[fidx].m_plane.w = c;\n"
"						facesA[fidx].m_numIndices=3;\n"
"					}\n"
"					fidx++;\n"
"\n"
"					bool addEdgePlanes = true;\n"
"					if (addEdgePlanes)\n"
"					{\n"
"						int numVertices=3;\n"
"						int prevVertex = numVertices-1;\n"
"						for (int i=0;i<numVertices;i++)\n"
"						{\n"
"							float4 v0 = verticesA[i];\n"
"							float4 v1 = verticesA[prevVertex];\n"
"	                                                \n"
"							float4 edgeNormal = normalize(cross(normal,v1-v0));\n"
"							float c = -dot(edgeNormal,v0);\n"
"	\n"
"							facesA[fidx].m_numIndices = 2;\n"
"							facesA[fidx].m_indexOffset=curUsedIndices;\n"
"							indicesA[curUsedIndices++]=i;\n"
"							indicesA[curUsedIndices++]=prevVertex;\n"
"	                                                \n"
"							facesA[fidx].m_plane.x = edgeNormal.x;\n"
"							facesA[fidx].m_plane.y = edgeNormal.y;\n"
"							facesA[fidx].m_plane.z = edgeNormal.z;\n"
"							facesA[fidx].m_plane.w = c;\n"
"							fidx++;\n"
"							prevVertex = i;\n"
"						}\n"
"					}\n"
"					convexPolyhedronA.m_numFaces = TRIANGLE_NUM_CONVEX_FACES;\n"
"					convexPolyhedronA.m_localCenter = localCenter*(1.f/3.f);\n"
"			\n"
"\n"
"					float4 posA = rigidBodies[bodyIndexA].m_pos;\n"
"					posA.w = 0.f;\n"
"					float4 posB = rigidBodies[bodyIndexB].m_pos;\n"
"					posB.w = 0.f;\n"
"					float4 c0local = convexPolyhedronA.m_localCenter;\n"
"					float4 ornA = rigidBodies[bodyIndexA].m_quat;\n"
"					float4 c0 = transform(&c0local, &posA, &ornA);\n"
"					float4 c1local = convexShapes[shapeIndexB].m_localCenter;\n"
"					float4 ornB =rigidBodies[bodyIndexB].m_quat;\n"
"					float4 c1 = transform(&c1local,&posB,&ornB);\n"
"					const float4 DeltaC2 = c0 - c1;\n"
"					\n"
"					bool sepA = findSeparatingAxisLocalA(	&convexPolyhedronA, &convexShapes[shapeIndexB],rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																											rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																											DeltaC2,\n"
"																											verticesA,uniqueEdgesA,facesA,indicesA,\n"
"																											vertices,uniqueEdges,faces,indices,\n"
"																											&sepAxis,&dmin);\n"
"					hasSeparatingAxis = 4;\n"
"					if (!sepA)\n"
"					{\n"
"						hasSeparatingAxis = 0;\n"
"					} else\n"
"					{\n"
"						bool sepB = findSeparatingAxisLocalB(	&convexShapes[shapeIndexB],&convexPolyhedronA,rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																												rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																												DeltaC2,\n"
"																												vertices,uniqueEdges,faces,indices,\n"
"																												verticesA,uniqueEdgesA,facesA,indicesA,\n"
"																												&sepAxis,&dmin);\n"
"			\n"
"						if (!sepB)\n"
"						{\n"
"							hasSeparatingAxis = 0;\n"
"						} else\n"
"						{\n"
"							bool sepEE = findSeparatingAxisEdgeEdgeLocalA(	&convexPolyhedronA, &convexShapes[shapeIndexB],rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																												rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																												DeltaC2,\n"
"																												verticesA,uniqueEdgesA,facesA,indicesA,\n"
"																												vertices,uniqueEdges,faces,indices,\n"
"																												&sepAxis,&dmin);\n"
"				\n"
"							if (!sepEE)\n"
"							{\n"
"								hasSeparatingAxis = 0;\n"
"							} else\n"
"							{\n"
"								hasSeparatingAxis = 1;\n"
"							}\n"
"						}\n"
"					}	\n"
"#endif\n"
"					\n"
"					if (hasSeparatingAxis)\n"
"					{\n"
"						int pairIdx = atomic_inc(numConcavePairsOut);\n"
"						if (pairIdx<maxnumConcavePairsCapacity)\n"
"						{\n"
"							concavePairsOut[pairIdx].x = bodyIndexA;\n"
"							concavePairsOut[pairIdx].y = bodyIndexB;\n"
"							concavePairsOut[pairIdx].z = f;\n"
"							concavePairsOut[pairIdx].w = 3;\n"
"							sepAxis.w = dmin;\n"
"							concaveSeparatingNormalsOut[pairIdx]=sepAxis;\n"
"						}\n"
"					}\n"
"				}\n"
"			}\n"
"			//todo\n"
"			hasSeparatingAxis[i] = 0;\n"
"			return;\n"
"		}		\n"
"\n"
"		if ((collidables[collidableIndexA].m_shapeType!=SHAPE_CONVEX_HULL) ||(collidables[collidableIndexB].m_shapeType!=SHAPE_CONVEX_HULL))\n"
"		{\n"
"			hasSeparatingAxis[i] = 0;\n"
"			return;\n"
"		}\n"
"			\n"
"\n"
"//once the broadphase avoids static-static pairs, we can remove this test\n"
"		if ((rigidBodies[bodyIndexA].m_invMass==0) &&(rigidBodies[bodyIndexB].m_invMass==0))\n"
"		{\n"
"			hasSeparatingAxis[i] = 0;\n"
"			return;\n"
"		}\n"
"\n"
"\n"
"		\n"
"		int numFacesA = convexShapes[shapeIndexA].m_numFaces;\n"
"\n"
"		float dmin = FLT_MAX;\n"
"\n"
"		float4 posA = rigidBodies[bodyIndexA].m_pos;\n"
"		posA.w = 0.f;\n"
"		float4 posB = rigidBodies[bodyIndexB].m_pos;\n"
"		posB.w = 0.f;\n"
"		float4 c0local = convexShapes[shapeIndexA].m_localCenter;\n"
"		float4 ornA = rigidBodies[bodyIndexA].m_quat;\n"
"		float4 c0 = transform(&c0local, &posA, &ornA);\n"
"		float4 c1local = convexShapes[shapeIndexB].m_localCenter;\n"
"		float4 ornB =rigidBodies[bodyIndexB].m_quat;\n"
"		float4 c1 = transform(&c1local,&posB,&ornB);\n"
"		const float4 DeltaC2 = c0 - c1;\n"
"		\n"
"		bool sepA = findSeparatingAxis(	&convexShapes[shapeIndexA], &convexShapes[shapeIndexB],rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																								rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																								DeltaC2,\n"
"																								vertices,uniqueEdges,faces,\n"
"																								indices,&separatingNormals[i],&dmin);\n"
"		hasSeparatingAxis[i] = 4;\n"
"		if (!sepA)\n"
"		{\n"
"			hasSeparatingAxis[i] = 0;\n"
"		} else\n"
"		{\n"
"			bool sepB = findSeparatingAxis(	&convexShapes[shapeIndexB],&convexShapes[shapeIndexA],rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																									rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																									DeltaC2,\n"
"																									vertices,uniqueEdges,faces,\n"
"																									indices,&separatingNormals[i],&dmin);\n"
"\n"
"			if (!sepB)\n"
"			{\n"
"				hasSeparatingAxis[i] = 0;\n"
"			} else\n"
"			{\n"
"				bool sepEE = findSeparatingAxisEdgeEdge(	&convexShapes[shapeIndexA], &convexShapes[shapeIndexB],rigidBodies[bodyIndexA].m_pos,rigidBodies[bodyIndexA].m_quat,\n"
"																									rigidBodies[bodyIndexB].m_pos,rigidBodies[bodyIndexB].m_quat,\n"
"																									DeltaC2,\n"
"																									vertices,uniqueEdges,faces,\n"
"																									indices,&separatingNormals[i],&dmin);\n"
"				if (!sepEE)\n"
"				{\n"
"					hasSeparatingAxis[i] = 0;\n"
"				} else\n"
"				{\n"
"					hasSeparatingAxis[i] = 1;\n"
"				}\n"
"			}\n"
"		}\n"
"		\n"
"	}\n"
"\n"
"}\n"
;
