#ifndef ROEHACDCOMPOUNDSHAPE_H_
#define ROEHACDCOMPOUNDSHAPE_H_

#include <btBulletDynamicsCommon.h>
#include <string>
#include "RoeHACD.h"

// Does NOT delete child shape on exiting (user may want to reuse them for other shapes, like when making a btCompoundShape out of uniform scaled children of this shape)
ATTRIBUTE_ALIGNED16(class) btHACDCompoundShape : public btCompoundShape
{
public:
	
	struct Params {
		// ORIGINAL PARAMS FROM CURRENT HACD SVN
		size_t maxHullVertices;            // (16) Max number of vertices in each convex hull sub shape. This is the only parameter that can be tweaked in many cases...
		double concavity;                // (200) Higher seems to reduce the number of sub shapes and viceversa. Can be used to increase or decrease the number of sub shapes.
		size_t nClusters;                // (1) Min number of resulting sub shapes. Can be used to increase the number of sub shapes.
		bool addExtraDistPoints;        // (false) Specifies whether extra points should be added when computing the concavity. Meshes with holes may need this param to be true.
		bool addNeighboursDistPoints;    // (false) Specifies whether extra points should be added when computing the concavity. Meshes with holes may need this param to be true.
		bool addFacesPoints;            // (false) Specifies whether faces points should be added when computing the concavity. Meshes with holes may need this param to be true.       
		
		double compacityWeight;            // (0.1) Sets the compacity weight (i.e. parameter alpha in ftp://ftp.elet.polimi.it/users/Stefano.Tubaro/ICIP_USB_Proceedings_v2/pdfs/0003501.pdf). = 0.1;
		double volumeWeight;            // (0.0) Sets the volume weight (i.e. parameter beta). = 0.0;
		std::string optionalVRMLSaveFilePath;    // (""). when set, it's supposed to save the decomposed mesh in VRML 2.0 format. Ignored if "keepSubmeshesSeparated" is true.
		double connectionDistance;        // (30.0) maximum distance to get CCs connected. Can be increased to decrease the number of convex hulls when increasing "concavity" has no effect. Very slow.    
		bool displayDebugInfo;            // (false) With printf
		double scaleFactor;                // (1000.0) (undocumented). DO NOT USE IT TO SCALE YOUR MESH!
		
		// W.I.P.: ADDITIONAL PARAMS ADDED BY ME (TO BE TESTED)
		btAlignedObjectArray< int > decomposeOnlySelectedSubmeshes;    // if size()==0, decompose all submeshes
		bool keepSubmeshesSeparated;                                    // (false). True is slower, and must be used when submesh index is needed.
		// A btAlignedObjectArray< int > submeshIndexOfChildShapes can be retrieved from the class instance to map the child shape index vs the submesh index.
		bool decomposeACleanCopyOfTheMesh;                            // (true). Handles duplicated vertices and degenerate triangles. Slower.
		bool decomposeADecimatedCopyOfTheMesh;                        // (false). True can be used to reduce the number of child shapes when increasing "concavity" and "connectionDist" does not help (but some detail gets lost). Slow? Maybe, but can speed up the decomposition process.
		float decimationDistanceInAabbHalfExtentsUnits;                // (0.1). In (0,1]. Bigger results in bigger decimation (= possibly less child shapes)
		bool decimationDistanceUniformInXYZ;                        // (true). When true: DDx = DDy = DDz = decimationDistanceInHalfAabbExtentsUnits * min(AabbHalfExtents)xyz;
		//           When false:  DDx = decimationDistanceInHalfAabbExtentsUnits * AabbHalfExtents.x;    
		//                        DDy = decimationDistanceInHalfAabbExtentsUnits * AabbHalfExtents.y;    
		//                        DDz = decimationDistanceInHalfAabbExtentsUnits * AabbHalfExtents.z;    
		bool decomposeACenteredCopyOfTheMesh;                        // (false). The mesh center is always calculated keeping into account all the subparts of the whole btStridingMeshInterface.
		btVector3 decomposeAScaledCopyOfTheMesh;                    // btVector3(1,1,1).
		std::string optionalBCSSaveFilePath;                        // (""). when set, the Bullet Collision Shape is saved as btCompoundShape to disk (*).
		float convexHullsCollisionMargin;                            // (0.01f) 
		bool convexHullsEnablePolyhedralContactClipping;            // (false) better quality, slightly slower
		bool shrinkObjectInwardsToCompensateCollisionMargin;        // (false)    // Based on the code in appConvexDecompositionDemo. Slow. And seems to produce artifacts...
		
		// (*) To load a btCollisionShape saved to disk:
		/*
    #include <BulletWorldImporter/btBulletWorldImporter.h>        // In "Bullet/Extra/Serialize". Needs link to: BulletWorldImporter.lib
    btCollisionShape* Load(const char* filename,bool verbose)    {    
        btBulletWorldImporter loader(0);//don't store info into the world
        loader.setVerboseMode(verbose);
        if (!loader.loadFile(filename)) return NULL;
    
        btCollisionShape* shape = NULL;
        if (loader.getNumCollisionShapes()>0) shape = loader.getCollisionShapeByIndex(0);
    
        //TODO: Cleaner way:
        // 1) Deep clone Collision Shape    
        // 2) loader.deleteAllData(); // deletes all (collision shapes included)
        // 3) return Deep cloned Collision Shape
    
        // Here we don't delete all data from the loader. (leaks?)
        return shape;    
    }    
		 */
		void reset() {*this = Params();}
		void display(bool modifiedParamsOnly=true);
		Params();
	};
	
	btHACDCompoundShape(const btStridingMeshInterface* stridingMeshInterface,const Params& params=Params());
	virtual ~btHACDCompoundShape() {}
	
	int getSubmeshIndexOfChildShape(int childShape) const {
		return m_submeshIndexOfChildShapes.size()>childShape ? m_submeshIndexOfChildShapes[childShape] : -1;
	}
	const btAlignedObjectArray< int >& getSubmeshIndexOfChildShapesMap() const {return m_submeshIndexOfChildShapes;}
	btAlignedObjectArray< int >& getSubmeshIndexOfChildShapesMap() {return m_submeshIndexOfChildShapes;}
	
	inline static btHACDCompoundShape* upcast(btCollisionShape* c) {return dynamic_cast < btHACDCompoundShape* > (c);}
	inline static const btHACDCompoundShape* upcast(const btCollisionShape* c) {return dynamic_cast < const btHACDCompoundShape* > (c);}
	
protected:
	//for easy inheriting:
	btHACDCompoundShape() {}
	
	virtual const char* getName() const {return "btHACDCompoundShape";}
	
	btAlignedObjectArray< int > m_submeshIndexOfChildShapes;
	
	static void GetStridingInterfaceContentOfSingleSubmesh(const btStridingMeshInterface* sti,std::vector< HACD::Vec3<HACD::Real> >& vertsOut,std::vector< HACD::Vec3<long> >& trianglesOut,int subMeshIndex);
	static void GetStridingInterfaceContent(const btStridingMeshInterface* sti,std::vector< HACD::Vec3<HACD::Real> >& vertsOut,std::vector< HACD::Vec3<long> >& trianglesOut,const btAlignedObjectArray< int >* processOnlySelectedSubmeshesInAscendingOrder=NULL);
	static HACD::Vec3<HACD::Real> GetAabbHalfExtentsFromStridingInterface(const btStridingMeshInterface* sti,HACD::Vec3<HACD::Real>* pOptionalCenterPointOut=NULL,const HACD::Vec3<HACD::Real>& scalingFactorToApply=HACD::Vec3<HACD::Real>(1,1,1));	
	static void ScalePoints(std::vector < HACD::Vec3<HACD::Real> >& verts,const HACD::Vec3<HACD::Real>& scalingFactor)    {
		for (size_t i=0,sz=verts.size();i<sz;i++)    {
			HACD::Vec3<HACD::Real>& v = verts[i];
			v.X()*=scalingFactor.X();
			v.Y()*=scalingFactor.Y();
			v.Z()*=scalingFactor.Z();
		}
	}
	static void ShiftPoints(std::vector < HACD::Vec3<HACD::Real> >& verts,const HACD::Vec3<HACD::Real>& shift)    {
		for (size_t i=0,sz=verts.size();i<sz;i++)    {
			HACD::Vec3<HACD::Real>& v = verts[i];
			v+=shift;
		}
	}
	
	struct Less{
		inline bool operator()(const int& a,const int& b) const {return a<b;}
	};
	
	inline static bool AreEqual(const HACD::Real s1,const HACD::Real s2,const HACD::Real eps=SIMD_EPSILON)    {
		return fabs(s1-s2)< eps;
	}
	
	inline static bool AreEqual(const HACD::Vec3<HACD::Real>& v1,const HACD::Vec3<HACD::Real>& v2,const HACD::Vec3<HACD::Real>& eps=HACD::Vec3<HACD::Real>(SIMD_EPSILON,SIMD_EPSILON,SIMD_EPSILON) )    {
		return (AreEqual(v1.X(),v2.X(),eps.X()) && AreEqual(v1.Y(),v2.Y(),eps.Y()) && AreEqual(v1.Z(),v2.Z(),eps.Z()) );
	}
	
	// This same method can be used to decimate the mesh too. Note that if the mesh has texcoords or other additional data, these will be broken after this method is called.
	void RemoveDoubleVertices(std::vector< HACD::Vec3<HACD::Real> >& verts,std::vector< HACD::Vec3<long> >& triangles,const bool removeDegenerateTrianglesToo=false,const HACD::Vec3<HACD::Real>& eps=HACD::Vec3<HACD::Real>(SIMD_EPSILON,SIMD_EPSILON,SIMD_EPSILON));
	
	static void HACDCallBackFunction(const char * msg, double progress, double concavity, size_t nVertices)
	{
		printf("%s",msg);
	}
	
};


#endif /* ROEHACDCOMPOUNDSHAPE_H_ */
