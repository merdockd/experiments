
#include "BulletDataExtractor.h"
#include "BulletSerialize/BulletFileLoader/btBulletFile.h"

#include <stdio.h>

#include <GL/glew.h>
#include "GLInstancingRenderer.h"
#include "CLPhysicsDemo.h"
//#include "LinearMath/btQuickprof.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"
#include "../opencl/gpu_rigidbody_pipeline/btConvexUtility.h"
#include "ShapeData.h"
///work-in-progress 
///This ReadBulletSample is kept as simple as possible without dependencies to the Bullet SDK.
///It can be used to load .bullet data for other physics SDKs
///For a more complete example how to load and convert Bullet data using the Bullet SDK check out
///the Bullet/Demos/SerializeDemo and Bullet/Serialize/BulletWorldImporter


//using namespace Bullet;

struct GraphicsVertex
{
	float xyzw[4];
	float normal[3];
	float uv[2];
};
struct GraphicsShape
{
	const float*	m_vertices;
	int				m_numvertices;
	const int*		m_indices;
	int				m_numIndices;
	float			m_scaling[4];
};

struct InstanceGroup
{
	Bullet::btCollisionShapeData* m_shape;
	int		m_collisionShapeIndex;

	btAlignedObjectArray<bParse::bStructHandle*> m_rigidBodies;
};


int NUM_OBJECTS_X = 35;
int NUM_OBJECTS_Y = 35;
int NUM_OBJECTS_Z = 35;


float X_GAP = 2.3f;
float Y_GAP = 2.f;
float Z_GAP = 2.3f;





void createSceneProgrammatically(GLInstancingRenderer& renderer,CLPhysicsDemo& physicsSim)
{
	int strideInBytes = sizeof(float)*9;

	bool noHeightField = true;
	int barrelShapeIndex = -1;
	int cubeShapeIndex = -1;
	int tetraShapeIndex = -1;

	float position[4]={0,0,0,0};
	btQuaternion born(btVector3(1,0,0),SIMD_PI*0.25*0.5);

	float orn[4] = {0,0,0,1};
//	float rotOrn[4] = {born.getX(),born.getY(),born.getZ(),born.getW()};//
	float rotOrn[4] ={0,0,0,1};
	

	float color[4] = {1,1,1,1};
	int index=0;
#if 0
	{
		int numVertices = sizeof(barrel_vertices)/strideInBytes;
		int numIndices = sizeof(barrel_indices)/sizeof(int);
		barrelShapeIndex = renderer.registerShape(&barrel_vertices[0],numVertices,barrel_indices,numIndices);
	}


	float barrelScaling[4] = {1,1,1,1};


	int barrelCollisionShapeIndex = physicsSim.registerCollisionShape(&barrel_vertices[0],strideInBytes, sizeof(barrel_vertices)/strideInBytes,&barrelScaling[0],noHeightField);
	


	for (int i=0;i<NUM_OBJECTS_X;i++)
	{
		for (int j=0;j<(NUM_OBJECTS_Y);j++)
		{
			for (int k=0;k<NUM_OBJECTS_Z;k++)
			{
				float mass = j? 1.f : 0.f;

				position[0]=(i*X_GAP-NUM_OBJECTS_X/2)+5;
				position[1]=1+j*Y_GAP*0.5;
				position[2]=(k*Z_GAP-NUM_OBJECTS_Z/2);
				position[3] = 1.f;
				
				renderer.registerGraphicsInstance(barrelShapeIndex,position,rotOrn,color,barrelScaling);
				void* ptr = (void*) index;
				physicsSim.registerPhysicsInstance(mass,  position, rotOrn, barrelCollisionShapeIndex,ptr);
				
				index++;
			}
		}
	}
#endif

	float cubeScaling[4] = {1,1,1,1};
	int cubeCollisionShapeIndex = physicsSim.registerCollisionShape(&cube_vertices[0],strideInBytes, sizeof(cube_vertices)/strideInBytes,&cubeScaling[0],noHeightField);


	{
		int numVertices = sizeof(cube_vertices)/strideInBytes;
		int numIndices = sizeof(cube_indices)/sizeof(int);
		cubeShapeIndex = renderer.registerShape(&cube_vertices[0],numVertices,cube_indices,numIndices);
	}

	
	



	if (1)
	for (int i=0;i<NUM_OBJECTS_X;i++)
	{
		for (int j=0;j<NUM_OBJECTS_Y;j++)
		{
			int k=0;
			
			for (;k<NUM_OBJECTS_Z;k++)
			{

				float mass = 1.f;//j? 1.f : 0.f;

				position[0]=(i*X_GAP-NUM_OBJECTS_X/2)+(j&1);
				position[1]=1+(j*Y_GAP);//-NUM_OBJECTS_Y/2);
				position[2]=(k*Z_GAP-NUM_OBJECTS_Z/2)+(j&1);
				position[3] = 0.f;
				
				renderer.registerGraphicsInstance(cubeShapeIndex,position,rotOrn,color,cubeScaling);
				int index1 = index;
				void* ptr = (void*) index1;
				physicsSim.registerPhysicsInstance(mass,  position, rotOrn, cubeCollisionShapeIndex,ptr);
				
				index++;
			}
		}
	}

	if (0)
	{
		//add some 'special' plane shape
		void* ptr = (void*) index;
		position[0] = 0.f;
		position[1] = 0.f;//-NUM_OBJECTS_Y/2-1;
		position[2] = 0.f;
		position[3] = 1.f;

		physicsSim.registerPhysicsInstance(0.f,position, orn, -1,ptr);
		color[0] = 1.f;
		color[1] = 0.f;
		color[2] = 0.f;
		cubeScaling[0] = 5000.f;
		cubeScaling[1] = 0.01f;
		cubeScaling[2] = 5000.f;

		renderer.registerGraphicsInstance(cubeShapeIndex,position,orn,color,cubeScaling);
	}

	{
		int numVertices = sizeof(tetra_vertices)/strideInBytes;
		int numIndices = sizeof(tetra_indices)/sizeof(int);
		tetraShapeIndex = renderer.registerShape(&tetra_vertices[0],numVertices,tetra_indices,numIndices);
	}

#if 1

	{
		float groundScaling[4] = {2.5,2,2.5,1};
		bool noHeightField = true;
		int cubeCollisionShapeIndex2 = physicsSim.registerCollisionShape(&tetra_vertices[0],strideInBytes, sizeof(tetra_vertices)/strideInBytes,&groundScaling[0],noHeightField);

		for (int i=0;i<50;i++)
			for (int j=0;j<50;j++)
		if (1)
		{
			void* ptr = (void*) index;
			float posnew[4];
			posnew[0] = i*5.01-120;
			posnew[1] = 0;
			posnew[2] = j*5.01-120;
			posnew[3] = 1.f;

			physicsSim.registerPhysicsInstance(0,  posnew, orn, cubeCollisionShapeIndex2,ptr);
			color[0] = 1.f;
			color[1] = 0.f;
			color[2] = 0.f;
			renderer.registerGraphicsInstance(tetraShapeIndex,posnew,orn,color,groundScaling);
		}
	}
#endif

	physicsSim.writeBodiesToGpu();


}


void createScene(GLInstancingRenderer& renderer,CLPhysicsDemo& physicsSim)
{
	//const char* fileName="../../bin/convex-trimesh.bullet";
	//const char* fileName="../../bin/1000 convex.bullet";
	const char* fileName="../../bin/1000 stack.bullet";
	//const char* fileName="../../bin/3000 fall.bullet";
	

	//const char* fileName="../../bin/testFile.bullet";
	

	
	FILE* f = fopen(fileName,"rb");
	if (f)
	{
		fclose(f);
		
		bool verboseDumpAllTypes = false;

		bParse::btBulletFile* bulletFile2 = new bParse::btBulletFile(fileName);

		bool ok = (bulletFile2->getFlags()& bParse::FD_OK)!=0;
	
		if (ok)
			bulletFile2->parse(verboseDumpAllTypes);
		else
		{
			printf("Error loading file %s.\n",fileName);
			exit(0);
		}
		ok = (bulletFile2->getFlags()& bParse::FD_OK)!=0;

		if (!(bulletFile2->getFlags() & bParse::FD_DOUBLE_PRECISION))
		{
			if (!ok)
			{
				printf("Error parsing file %s.\n",fileName);
				exit(0);
			}
	
			if (verboseDumpAllTypes)
			{
				bulletFile2->dumpChunks(bulletFile2->getFileDNA());
			}


			btBulletDataExtractor extractor(renderer,physicsSim);
	
			extractor.convertAllObjects(bulletFile2);
			delete bulletFile2;
			return;

		} else
		{
			printf("Error: double precision .bullet files not supported in this demo\n");
		}

		delete bulletFile2;
	} else
	{
		printf("Warning: cannot find file %s, using programmatically created scene instead.\n",fileName);
	}


	
	
	createSceneProgrammatically(renderer,physicsSim);
	
	
}



enum LocalBroadphaseNativeTypes
{
	// polyhedral convex shapes
	BOX_SHAPE_PROXYTYPE,
	TRIANGLE_SHAPE_PROXYTYPE,
	TETRAHEDRAL_SHAPE_PROXYTYPE,
	CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE,
	CONVEX_HULL_SHAPE_PROXYTYPE,
	CONVEX_POINT_CLOUD_SHAPE_PROXYTYPE,
	CUSTOM_POLYHEDRAL_SHAPE_TYPE,
//implicit convex shapes
IMPLICIT_CONVEX_SHAPES_START_HERE,
	SPHERE_SHAPE_PROXYTYPE,
	MULTI_SPHERE_SHAPE_PROXYTYPE,
	CAPSULE_SHAPE_PROXYTYPE,
	CONE_SHAPE_PROXYTYPE,
	CONVEX_SHAPE_PROXYTYPE,
	CYLINDER_SHAPE_PROXYTYPE,
	UNIFORM_SCALING_SHAPE_PROXYTYPE,
	MINKOWSKI_SUM_SHAPE_PROXYTYPE,
	MINKOWSKI_DIFFERENCE_SHAPE_PROXYTYPE,
	BOX_2D_SHAPE_PROXYTYPE,
	CONVEX_2D_SHAPE_PROXYTYPE,
	CUSTOM_CONVEX_SHAPE_TYPE,
//concave shapes
CONCAVE_SHAPES_START_HERE,
	//keep all the convex shapetype below here, for the check IsConvexShape in broadphase proxy!
	TRIANGLE_MESH_SHAPE_PROXYTYPE,
	SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE,
	///used for demo integration FAST/Swift collision library and Bullet
	FAST_CONCAVE_MESH_PROXYTYPE,
	//terrain
	TERRAIN_SHAPE_PROXYTYPE,
///Used for GIMPACT Trimesh integration
	GIMPACT_SHAPE_PROXYTYPE,
///Multimaterial mesh
    MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE,
	
	EMPTY_SHAPE_PROXYTYPE,
	STATIC_PLANE_PROXYTYPE,
	CUSTOM_CONCAVE_SHAPE_TYPE,
CONCAVE_SHAPES_END_HERE,

	COMPOUND_SHAPE_PROXYTYPE,

	SOFTBODY_SHAPE_PROXYTYPE,
	HFFLUID_SHAPE_PROXYTYPE,
	HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE,
	INVALID_SHAPE_PROXYTYPE,

	MAX_BROADPHASE_COLLISION_TYPES
	
};

btBulletDataExtractor::btBulletDataExtractor(GLInstancingRenderer&	renderer, CLPhysicsDemo&	physicsSim)
	:m_renderer(renderer), m_physicsSim(physicsSim)
{
}

btBulletDataExtractor::~btBulletDataExtractor()
{
}



void btBulletDataExtractor::convertAllObjects(bParse::btBulletFile* bulletFile2)
{
	int i;

	for (i=0;i<bulletFile2->m_collisionShapes.size();i++)
	{
		Bullet::btCollisionShapeData* shapeData = (Bullet::btCollisionShapeData*)bulletFile2->m_collisionShapes[i];
		if (shapeData->m_name)
			printf("converting shape %s\n", shapeData->m_name);
		int shapeIndex = convertCollisionShape(shapeData);
		//valid conversion?
		if (shapeIndex>=0)
		{
			InstanceGroup* group = new InstanceGroup;
			group->m_shape = shapeData;
			group->m_collisionShapeIndex = shapeIndex;
			m_instanceGroups.push_back(group);

		}
	}

	for (i=0;i<bulletFile2->m_rigidBodies.size();i++)
	{
		
		Bullet::btRigidBodyFloatData* colObjData = (Bullet::btRigidBodyFloatData*)bulletFile2->m_rigidBodies[i];
		Bullet::btCollisionShapeData* shapeData = (Bullet::btCollisionShapeData*)colObjData->m_collisionObjectData.m_collisionShape;
		for (int j=0;j<m_instanceGroups.size();j++)
		{
			if (m_instanceGroups[j]->m_shape == shapeData)
			{
				m_instanceGroups[j]->m_rigidBodies.push_back(bulletFile2->m_rigidBodies[i]);
			}
		}
	}

	//now register all objects in order
	for (int i=0;i<m_instanceGroups.size();i++)
	{
		if (m_instanceGroups[i]->m_rigidBodies.size()>0)
		{

			m_renderer.registerShape(m_graphicsShapes[i]->m_vertices,m_graphicsShapes[i]->m_numvertices,m_graphicsShapes[i]->m_indices,m_graphicsShapes[i]->m_numIndices);

			for (int j=0;j<m_instanceGroups[i]->m_rigidBodies.size();j++)
			{
				Bullet::btRigidBodyFloatData* colObjData = (Bullet::btRigidBodyFloatData*)m_instanceGroups[i]->m_rigidBodies[j];
				
				btMatrix3x3 mat;
				mat.deSerializeFloat((const btMatrix3x3FloatData&)colObjData->m_collisionObjectData.m_worldTransform.m_basis);
				btQuaternion orn;
				mat.getRotation(orn);
				float quaternion[4] = {orn[0],orn[1],orn[2],orn[3]};
				float pos[4] = {colObjData->m_collisionObjectData.m_worldTransform.m_origin.m_floats[0],
								colObjData->m_collisionObjectData.m_worldTransform.m_origin.m_floats[1],
								colObjData->m_collisionObjectData.m_worldTransform.m_origin.m_floats[2],
								0.f};
				float color[4] = {0,0,0,0};
				float mass = 0.f;
				if (colObjData->m_inverseMass==0.f)
				{
					color[1] = 1;
				} else
				{
					mass = 1.f/colObjData->m_inverseMass;
					color[0] = 1;
				void* ptr = (void*) m_physicsSim.m_numPhysicsInstances;
				m_physicsSim.registerPhysicsInstance(mass,pos,quaternion,m_instanceGroups[i]->m_collisionShapeIndex,ptr);
				m_renderer.registerGraphicsInstance(m_instanceGroups[i]->m_collisionShapeIndex,pos,quaternion,color,m_graphicsShapes[i]->m_scaling);
	
				}

		
				
			}
		}
	}

	for (i=0;i<bulletFile2->m_collisionObjects.size();i++)
	{
		
	}

	#if 1

	{
		int tetraShapeIndex= -1;
		int strideInBytes = sizeof(float)*9;
		{
		int numVertices = sizeof(tetra_vertices)/strideInBytes;
		int numIndices = sizeof(tetra_indices)/sizeof(int);
		tetraShapeIndex = m_renderer.registerShape(&tetra_vertices[0],numVertices,tetra_indices,numIndices);
		}


		float groundScaling[4] = {2.5,2,2.5,1};
		bool noHeightField = true;
		int cubeCollisionShapeIndex2 = m_physicsSim.registerCollisionShape(&tetra_vertices[0],strideInBytes, sizeof(tetra_vertices)/strideInBytes,&groundScaling[0],noHeightField);

		for (int i=0;i<50;i++)
			for (int j=0;j<50;j++)
		if (1)
		{
			
			void* ptr = (void*) m_physicsSim.m_numPhysicsInstances;
			float posnew[4];
			posnew[0] = i*5.01-120;
			posnew[1] = 0;
			posnew[2] = j*5.01-120;
			posnew[3] = 1.f;
			float orn[4] = {0,0,0,1};
			float color[4] = {0,0,1,1};

			m_physicsSim.registerPhysicsInstance(0,  posnew, orn, cubeCollisionShapeIndex2,ptr);
			m_renderer.registerGraphicsInstance(tetraShapeIndex,posnew,orn,color,groundScaling);
		}
	}
#endif


	m_physicsSim.writeBodiesToGpu();
}



int btBulletDataExtractor::convertCollisionShape(  Bullet::btCollisionShapeData* shapeData  )
{
	int shapeIndex = -1;

	switch (shapeData->m_shapeType)
		{
	case STATIC_PLANE_PROXYTYPE:
		{
			Bullet::btStaticPlaneShapeData* planeData = (Bullet::btStaticPlaneShapeData*)shapeData;
			shapeIndex = createPlaneShape(planeData->m_planeNormal,planeData->m_planeConstant, planeData->m_localScaling);
			break;
		}

		case CYLINDER_SHAPE_PROXYTYPE:
		case CAPSULE_SHAPE_PROXYTYPE:
		case BOX_SHAPE_PROXYTYPE:
		case SPHERE_SHAPE_PROXYTYPE:
		case MULTI_SPHERE_SHAPE_PROXYTYPE:
		case CONVEX_HULL_SHAPE_PROXYTYPE:
			{
				Bullet::btConvexInternalShapeData* bsd = (Bullet::btConvexInternalShapeData*)shapeData;
				
				switch (shapeData->m_shapeType)
				{
					case BOX_SHAPE_PROXYTYPE:
						{
							shapeIndex = createBoxShape(bsd->m_implicitShapeDimensions, bsd->m_localScaling,bsd->m_collisionMargin);
							break;
						}
					case SPHERE_SHAPE_PROXYTYPE:
						{
							shapeIndex = createSphereShape(bsd->m_implicitShapeDimensions.m_floats[0],bsd->m_localScaling, bsd->m_collisionMargin);
							break;
						}
					case CONVEX_HULL_SHAPE_PROXYTYPE:
						{
							Bullet::btConvexHullShapeData* convexData = (Bullet::btConvexHullShapeData*)bsd;
							int numPoints = convexData->m_numUnscaledPoints;
							

							btVector3 localScaling;
							localScaling.deSerializeFloat((btVector3FloatData&)bsd->m_localScaling);
							btAlignedObjectArray<btVector3> tmpPoints;
							int i;
							if (convexData->m_unscaledPointsFloatPtr)
							{
								for ( i=0;i<numPoints;i++)
								{
									btVector3 pt = btVector3(convexData->m_unscaledPointsFloatPtr[i].m_floats[0],
										convexData->m_unscaledPointsFloatPtr[i].m_floats[1],
										convexData->m_unscaledPointsFloatPtr[i].m_floats[2]);//convexData->m_unscaledPointsFloatPtr[i].m_floats[3]);
									
									tmpPoints.push_back(pt*localScaling);
								}
							}

							float unitScaling[4] = {1,1,1,1};

							int strideInBytes = sizeof(btVector3);
							strideInBytes = 4*sizeof(float);
							int noHeightField = 1;
							shapeIndex  = m_physicsSim.registerCollisionShape(&tmpPoints[0].m_floats[0],strideInBytes, numPoints,&unitScaling[0],noHeightField);

							btConvexUtility* utilPtr = new btConvexUtility();
							bool merge = true;
							utilPtr->initializePolyhedralFeatures(tmpPoints,merge);
							
							btAlignedObjectArray<GraphicsVertex>* vertices = new btAlignedObjectArray<GraphicsVertex>;
							{
								int numVertices = utilPtr->m_vertices.size();
								int numIndices = 0;
								btAlignedObjectArray<int>* indicesPtr = new btAlignedObjectArray<int>;
								for (int f=0;f<utilPtr->m_faces.size();f++)
								{
									const btFace& face = utilPtr->m_faces[f];
									btVector3 normal(face.m_plane[0],face.m_plane[1],face.m_plane[2]);
									if (face.m_indices.size()>2)
									{
										
										GraphicsVertex vtx;
										const btVector3& orgVertex = utilPtr->m_vertices[face.m_indices[0]];
										vtx.xyzw[0] = orgVertex[0];vtx.xyzw[1] = orgVertex[1];vtx.xyzw[2] = orgVertex[2];vtx.xyzw[3] = 0.f;
										vtx.normal[0] = normal[0];vtx.normal[1] = normal[1];vtx.normal[2] = normal[2];
										vtx.uv[0] = 0.5f;vtx.uv[1] = 0.5f;
										int newvtxindex0 = vertices->size();
										vertices->push_back(vtx);
									
										for (int j=1;j<face.m_indices.size()-1;j++)
										{
											indicesPtr->push_back(newvtxindex0);
											{
												GraphicsVertex vtx;
												const btVector3& orgVertex = utilPtr->m_vertices[face.m_indices[j]];
												vtx.xyzw[0] = orgVertex[0];vtx.xyzw[1] = orgVertex[1];vtx.xyzw[2] = orgVertex[2];vtx.xyzw[3] = 0.f;
												vtx.normal[0] = normal[0];vtx.normal[1] = normal[1];vtx.normal[2] = normal[2];
												vtx.uv[0] = 0.5f;vtx.uv[1] = 0.5f;
												int newvtxindexj = vertices->size();
												vertices->push_back(vtx);
												indicesPtr->push_back(newvtxindexj);
											}

											{
												GraphicsVertex vtx;
												const btVector3& orgVertex = utilPtr->m_vertices[face.m_indices[j+1]];
												vtx.xyzw[0] = orgVertex[0];vtx.xyzw[1] = orgVertex[1];vtx.xyzw[2] = orgVertex[2];vtx.xyzw[3] = 0.f;
												vtx.normal[0] = normal[0];vtx.normal[1] = normal[1];vtx.normal[2] = normal[2];
												vtx.uv[0] = 0.5f;vtx.uv[1] = 0.5f;
												int newvtxindexj1 = vertices->size();
												vertices->push_back(vtx);
												indicesPtr->push_back(newvtxindexj1);
											}
										}
									}
								}
								
								
								GraphicsShape* gfxShape = new GraphicsShape;
								gfxShape->m_vertices = &vertices->at(0).xyzw[0];
								gfxShape->m_numvertices = vertices->size();
								gfxShape->m_indices = &indicesPtr->at(0);
								gfxShape->m_numIndices = indicesPtr->size();
								for (int i=0;i<4;i++)
									gfxShape->m_scaling[i] = 1;//bake the scaling into the vertices 
								m_graphicsShapes.push_back(gfxShape);
							}
							
							printf("createConvexHull with %d vertices\n",numPoints);

//							shape = createConvexHullShape();

							return shapeIndex;
							break;
						}
#if 0
					case CAPSULE_SHAPE_PROXYTYPE:
						{
							btCapsuleShapeData* capData = (btCapsuleShapeData*)shapeData;
							switch (capData->m_upAxis)
							{
							case 0:
								{
									shape = createCapsuleShapeX(implicitShapeDimensions.getY(),2*implicitShapeDimensions.getX());
									break;
								}
							case 1:
								{
									shape = createCapsuleShapeY(implicitShapeDimensions.getX(),2*implicitShapeDimensions.getY());
									break;
								}
							case 2:
								{
									shape = createCapsuleShapeZ(implicitShapeDimensions.getX(),2*implicitShapeDimensions.getZ());
									break;
								}
							default:
								{
									printf("error: wrong up axis for btCapsuleShape\n");
								}

							};
							
							break;
						}
					case CYLINDER_SHAPE_PROXYTYPE:
						{
							btCylinderShapeData* cylData = (btCylinderShapeData*) shapeData;
							btVector3 halfExtents = implicitShapeDimensions+margin;
							switch (cylData->m_upAxis)
							{
							case 0:
								{
									shape = createCylinderShapeX(halfExtents.getY(),halfExtents.getX());
									break;
								}
							case 1:
								{
									shape = createCylinderShapeY(halfExtents.getX(),halfExtents.getY());
									break;
								}
							case 2:
								{
									shape = createCylinderShapeZ(halfExtents.getX(),halfExtents.getZ());
									break;
								}
							default:
								{
									printf("unknown Cylinder up axis\n");
								}

							};
							

							
							break;
						}
					case MULTI_SPHERE_SHAPE_PROXYTYPE:
						{
							btMultiSphereShapeData* mss = (btMultiSphereShapeData*)bsd;
							int numSpheres = mss->m_localPositionArraySize;
							int i;
							for ( i=0;i<numSpheres;i++)
							{
								tmpPos[i].deSerializeFloat(mss->m_localPositionArrayPtr[i].m_pos);
								radii[i] = mss->m_localPositionArrayPtr[i].m_radius;
							}
							shape = new btMultiSphereShape(&tmpPos[0],&radii[0],numSpheres);
							break;
						}
			
#endif

					default:
						{
							printf("error: cannot create shape type (%d)\n",shapeData->m_shapeType);
						}
				}

				break;
			}

		case TRIANGLE_MESH_SHAPE_PROXYTYPE:
		{
			Bullet::btTriangleMeshShapeData* trimesh = (Bullet::btTriangleMeshShapeData*)shapeData;
			printf("numparts = %d\n",trimesh->m_meshInterface.m_numMeshParts);
			if (trimesh->m_meshInterface.m_numMeshParts)
			{
				for (int i=0;i<trimesh->m_meshInterface.m_numMeshParts;i++)
				{
					Bullet::btMeshPartData& dat = trimesh->m_meshInterface.m_meshPartsPtr[i];
					printf("numtris = %d, numverts = %d\n", dat.m_numTriangles,dat.m_numVertices);//,dat.m_vertices3f,dat.m_3indices16
					printf("scaling = %f,%f,%f\n", trimesh->m_meshInterface.m_scaling.m_floats[0],trimesh->m_meshInterface.m_scaling.m_floats[1],trimesh->m_meshInterface.m_scaling.m_floats[2]);
					//	dat.
					//dat.

				}
					
				///trimesh->m_meshInterface.m_meshPartsPtr
				//trimesh->m_meshInterface.m_scaling
			}
			//trimesh->m_meshInterface
			//btTriangleIndexVertexArray* meshInterface = createMeshInterface(trimesh->m_meshInterface);
			

			//scaling
			//btVector3 scaling; scaling.deSerializeFloat(trimesh->m_meshInterface.m_scaling);
			//meshInterface->setScaling(scaling);

			//printf("trimesh->m_collisionMargin=%f\n",trimesh->m_collisionMargin);
			break;
		}

#if 0
		case COMPOUND_SHAPE_PROXYTYPE:
			{
				btCompoundShapeData* compoundData = (btCompoundShapeData*)shapeData;
				btCompoundShape* compoundShape = createCompoundShape();


				btAlignedObjectArray<btCollisionShape*> childShapes;
				for (int i=0;i<compoundData->m_numChildShapes;i++)
				{
					btCollisionShape* childShape = convertCollisionShape(compoundData->m_childShapePtr[i].m_childShape);
					if (childShape)
					{
						btTransform localTransform;
						localTransform.deSerializeFloat(compoundData->m_childShapePtr[i].m_transform);
						compoundShape->addChildShape(localTransform,childShape);
					} else
					{
						printf("error: couldn't create childShape for compoundShape\n");
					}
					
				}
				shape = compoundShape;

				break;
			}

			case GIMPACT_SHAPE_PROXYTYPE:
		{
			btGImpactMeshShapeData* gimpactData = (btGImpactMeshShapeData*) shapeData;
			if (gimpactData->m_gimpactSubType == CONST_GIMPACT_TRIMESH_SHAPE)
			{
				btTriangleIndexVertexArray* meshInterface = createMeshInterface(gimpactData->m_meshInterface);
				btGImpactMeshShape* gimpactShape = createGimpactShape(meshInterface);
				btVector3 localScaling;
				localScaling.deSerializeFloat(gimpactData->m_localScaling);
				gimpactShape->setLocalScaling(localScaling);
				gimpactShape->setMargin(btScalar(gimpactData->m_collisionMargin));
				gimpactShape->updateBound();
				shape = gimpactShape;
			} else
			{
				printf("unsupported gimpact sub type\n");
			}
			break;
		}
		case SOFTBODY_SHAPE_PROXYTYPE:
			{
				return 0;
			}
#endif 
		default:
			{
				printf("unsupported shape type (%d)\n",shapeData->m_shapeType);
			}
		}

		return shapeIndex;
	
}

int btBulletDataExtractor::createBoxShape( const Bullet::btVector3FloatData& halfDimensions, const Bullet::btVector3FloatData& localScaling, float collisionMargin)
{
	float cubeScaling[4] = {
		halfDimensions.m_floats[0]*localScaling.m_floats[0]+collisionMargin,
		halfDimensions.m_floats[1]*localScaling.m_floats[1]+collisionMargin,
		halfDimensions.m_floats[2]*localScaling.m_floats[2]+collisionMargin,
		1};
	int strideInBytes = sizeof(float)*9;
	int noHeightField = 1;
	int cubeCollisionShapeIndex = m_physicsSim.registerCollisionShape(&cube_vertices[0],strideInBytes, sizeof(cube_vertices)/strideInBytes,&cubeScaling[0],noHeightField);

	{
		int numVertices = sizeof(cube_vertices)/strideInBytes;
		int numIndices = sizeof(cube_indices)/sizeof(int);
		
		GraphicsShape* gfxShape = new GraphicsShape;
		gfxShape->m_vertices = cube_vertices;
		gfxShape->m_numvertices = numVertices;
		gfxShape->m_indices = cube_indices;
		gfxShape->m_numIndices = numIndices;
		for (int i=0;i<4;i++)
			gfxShape->m_scaling[i] = cubeScaling[i];
		m_graphicsShapes.push_back(gfxShape);
	}

	printf("createBoxShape with half extents %f,%f,%f\n",cubeScaling[0], cubeScaling[1],cubeScaling[2]);
	//halfDimensions * localScaling
	return cubeCollisionShapeIndex;
}

int btBulletDataExtractor::createSphereShape( float radius, const Bullet::btVector3FloatData& localScaling, float collisionMargin)
{
	printf("createSphereShape with radius %f\n",radius);
	return -1;
}


int btBulletDataExtractor::createPlaneShape( const Bullet::btVector3FloatData& planeNormal, float planeConstant, const Bullet::btVector3FloatData& localScaling)
{
	printf("createPlaneShape with normal %f,%f,%f and planeConstant\n",planeNormal.m_floats[0], planeNormal.m_floats[1],planeNormal.m_floats[2],planeConstant);
	return -1;
}
