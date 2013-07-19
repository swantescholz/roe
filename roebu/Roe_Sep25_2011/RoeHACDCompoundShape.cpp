#include "RoeHACDCompoundShape.h"
#include <LinearMath/btGeometryUtil.h>

//PARAMS================================
void btHACDCompoundShape::Params::display(bool modifiedParamsOnly) {
	Params d;
	if (!modifiedParamsOnly || d.maxHullVertices!=maxHullVertices)     printf("params.maxHullVertices    =                %1d\n",(int) maxHullVertices);
	if (!modifiedParamsOnly || d.concavity!=concavity)                 printf("params.concavity    =                %1.6f\n",concavity);
	if (!modifiedParamsOnly || d.nClusters!=nClusters)                 printf("params.nClusters    =                %1d\n",(int) nClusters);
	if (!modifiedParamsOnly || d.addExtraDistPoints!=addExtraDistPoints) {
		if (addExtraDistPoints)                                     printf("params.addExtraDistPoints    =            true\n");
		else                                                         printf("params.addExtraDistPoints    =            false\n");
	}    
	if (!modifiedParamsOnly || d.addNeighboursDistPoints!=addNeighboursDistPoints) {
		if (addNeighboursDistPoints)                                 printf("params.addNeighboursDistPoints    =            true\n");
		else                                                         printf("params.addNeighboursDistPoints    =            false\n");
	}    
	if (!modifiedParamsOnly || d.addFacesPoints!=addFacesPoints) {
		if (addFacesPoints)                                         printf("params.addFacesPoints    =                true\n");
		else                                                         printf("params.addFacesPoints    =                false\n");
	}
	
	if (!modifiedParamsOnly || d.compacityWeight!=compacityWeight)     printf("params.compacityWeight    =                %1.6f\n",compacityWeight);
	if (!modifiedParamsOnly || d.volumeWeight!=volumeWeight)         printf("params.volumeWeight    =                %1.6f\n",volumeWeight);
	if (!modifiedParamsOnly || optionalVRMLSaveFilePath.size()>0)                 printf("params.optionalVRMLSaveFilePath    =            \"%s\"\n",optionalVRMLSaveFilePath.c_str());
	if (!modifiedParamsOnly || d.connectionDistance!=connectionDistance)     printf("params.connectionDistance    =            %1.6f\n",connectionDistance);
	if (!modifiedParamsOnly || d.displayDebugInfo!=displayDebugInfo) {
		if (displayDebugInfo)                                         printf("params.displayDebugInfo    =                true\n");
		else                                                         printf("params.displayDebugInfo    =                false\n");
	}        
	if (!modifiedParamsOnly || d.scaleFactor!=scaleFactor)             printf("params.scaleFactor    =                %1.6f\n",scaleFactor);
	
	if (!modifiedParamsOnly || d.decomposeOnlySelectedSubmeshes.size()!=decomposeOnlySelectedSubmeshes.size())     {
		printf ("params.decomposeOnlySelectedSubmeshes    =        ");
		if (decomposeOnlySelectedSubmeshes.size()==0) printf("No, thanks\n");
		else    {
			printf("(");
			for (int i=0,sz=decomposeOnlySelectedSubmeshes.size();i<sz;i++)    {
				printf("%1d",decomposeOnlySelectedSubmeshes[i]);
				if (i!=sz-1) printf(",");
			}
			printf(")\n");    
		}    
	}
	if (!modifiedParamsOnly || d.keepSubmeshesSeparated!=keepSubmeshesSeparated) {
		if (keepSubmeshesSeparated)                                     printf("params.keepSubmeshesSeparated    =            true\n");
		else                                                         printf("params.keepSubmeshesSeparated    =            false\n");
	}        
	if (!modifiedParamsOnly || d.decomposeACleanCopyOfTheMesh!=decomposeACleanCopyOfTheMesh) {
		if (decomposeACleanCopyOfTheMesh)                             printf("params.decomposeACleanCopyOfTheMesh    =        true\n");
		else                                                         printf("params.decomposeACleanCopyOfTheMesh    =        false\n");
	}        
	if (!modifiedParamsOnly || d.decomposeADecimatedCopyOfTheMesh!=decomposeADecimatedCopyOfTheMesh) {
		if (decomposeADecimatedCopyOfTheMesh)                         printf("params.decomposeADecimatedCopyOfTheMesh    =        true\n");
		else                                                         printf("params.decomposeADecimatedCopyOfTheMesh    =        false\n");
	}        
	if (!modifiedParamsOnly || d.decimationDistanceInAabbHalfExtentsUnits!=decimationDistanceInAabbHalfExtentsUnits)     
		printf("params.decimationDistanceInAabbHalfExtentsUnits    =    %1.6f\n",decimationDistanceInAabbHalfExtentsUnits);        
	if (!modifiedParamsOnly || d.decimationDistanceUniformInXYZ!=decimationDistanceUniformInXYZ) {
		if (decimationDistanceUniformInXYZ)                         printf("params.decimationDistanceUniformInXYZ    =        true\n");
		else                                                         printf("params.decimationDistanceUniformInXYZ    =        false\n");
	}        
	if (!modifiedParamsOnly || d.decomposeACenteredCopyOfTheMesh!=decomposeACenteredCopyOfTheMesh) {
		if (decomposeACenteredCopyOfTheMesh)                         printf("params.decomposeACenteredCopyOfTheMesh    =        true\n");
		else                                                         printf("params.decomposeACenteredCopyOfTheMesh    =        false\n");
	}
	if (!modifiedParamsOnly || d.decomposeAScaledCopyOfTheMesh!=decomposeAScaledCopyOfTheMesh)         
		printf("params.decomposeAScaledCopyOfTheMesh    =        (%1.3f,%1.3f,%1.3f)\n",decomposeAScaledCopyOfTheMesh.x(),decomposeAScaledCopyOfTheMesh.y(),decomposeAScaledCopyOfTheMesh.z());
	if (!modifiedParamsOnly || optionalBCSSaveFilePath.size()>0)                 printf("params.optionalBCSSaveFilePath    =            \"%s\"\n",optionalBCSSaveFilePath.c_str());
	if (!modifiedParamsOnly || d.convexHullsCollisionMargin!=convexHullsCollisionMargin)         printf("params.convexHullsCollisionMargin    =        %1.3f\n",convexHullsCollisionMargin);
	if (!modifiedParamsOnly || d.convexHullsEnablePolyhedralContactClipping!=convexHullsEnablePolyhedralContactClipping) {
		if (convexHullsEnablePolyhedralContactClipping)             printf("params.convexHullsEnablePolyhedralContactClipping    =            true\n");
		else                                                         printf("params.convexHullsEnablePolyhedralContactClipping    =            false\n");
	}    
	if (!modifiedParamsOnly || d.shrinkObjectInwardsToCompensateCollisionMargin!=shrinkObjectInwardsToCompensateCollisionMargin) {
		if (shrinkObjectInwardsToCompensateCollisionMargin)         printf("params.shrinkObjectInwardsToCompensateCollisionMargin=    true\n");
		else                                                         printf("params.shrinkObjectInwardsToCompensateCollisionMargin=    false\n");
	}                                                
}

//============
// DEFAULTS:
//============
btHACDCompoundShape::Params::Params() :
				maxHullVertices(                    16),
				concavity(                            200),
				nClusters(                            1),
				addExtraDistPoints(                    false),
				addNeighboursDistPoints(            false),
				addFacesPoints(                        false),
				compacityWeight(                    0.1),
				volumeWeight(                        0.0),
				optionalVRMLSaveFilePath(            ""),
				connectionDistance(                    30.0),                
				displayDebugInfo(                    false),
				scaleFactor(                        1000.0),
				keepSubmeshesSeparated(                false),
				decomposeACleanCopyOfTheMesh(        true),
				decomposeADecimatedCopyOfTheMesh(    false),
				decimationDistanceInAabbHalfExtentsUnits(0.1f),
				decimationDistanceUniformInXYZ(        true),
				decomposeACenteredCopyOfTheMesh(    false),
				decomposeAScaledCopyOfTheMesh(        btVector3(1,1,1)),
				optionalBCSSaveFilePath(            ""),                
				convexHullsCollisionMargin(            0.01f),
				convexHullsEnablePolyhedralContactClipping(false),
				shrinkObjectInwardsToCompensateCollisionMargin(false)
{}

//MAIN==================================
btHACDCompoundShape::btHACDCompoundShape(const btStridingMeshInterface* stridingMeshInterface, const Params& params) {
	m_submeshIndexOfChildShapes.resize(0);
	if (!stridingMeshInterface) return;
	
	//const std::clock_t ck1 = std::clock();
	btAlignedObjectArray< int > decomposeOnlySelectedSubmeshes = params.decomposeOnlySelectedSubmeshes;
	if (decomposeOnlySelectedSubmeshes.size()) {
		decomposeOnlySelectedSubmeshes.quickSort(Less());
	}    
	
	const HACD::Vec3<HACD::Real> meshScalingFactor = HACD::Vec3<HACD::Real>(params.decomposeAScaledCopyOfTheMesh.x(),params.decomposeAScaledCopyOfTheMesh.y(),params.decomposeAScaledCopyOfTheMesh.z());
	const bool mustScaleMesh = (params.decomposeAScaledCopyOfTheMesh.x()!=1 || params.decomposeAScaledCopyOfTheMesh.y()!=1 || params.decomposeAScaledCopyOfTheMesh.z()!=1);
	HACD::Vec3<HACD::Real> aabbHalfExtents(0,0,0);
	HACD::Vec3<HACD::Real> aabbCenterPoint;
	if (params.decomposeADecimatedCopyOfTheMesh || params.decomposeACenteredCopyOfTheMesh)    {
		aabbHalfExtents = GetAabbHalfExtentsFromStridingInterface(stridingMeshInterface,&aabbCenterPoint,meshScalingFactor);
	}
	
	HACD::Vec3<HACD::Real> decimationDistance(SIMD_EPSILON,SIMD_EPSILON,SIMD_EPSILON);
	if (params.decomposeADecimatedCopyOfTheMesh)    {
		if (params.decimationDistanceUniformInXYZ) {
			const HACD::Real DD = params.decimationDistanceInAabbHalfExtentsUnits * (
					(aabbHalfExtents.X() < aabbHalfExtents.Y() && aabbHalfExtents.X() < aabbHalfExtents.Z()) ? aabbHalfExtents.X() :
					                                                                                         (aabbHalfExtents.Y() < aabbHalfExtents.X() && aabbHalfExtents.Y() < aabbHalfExtents.Z()) ? aabbHalfExtents.Y() :
					                                                                                                                                                                                  aabbHalfExtents.Z()
			);
			decimationDistance = HACD::Vec3<HACD::Real>(DD,DD,DD);
		}
		else decimationDistance = HACD::Vec3<HACD::Real>(params.decimationDistanceInAabbHalfExtentsUnits*aabbHalfExtents.X(),params.decimationDistanceInAabbHalfExtentsUnits*aabbHalfExtents.Y(),params.decimationDistanceInAabbHalfExtentsUnits*aabbHalfExtents.Z());                                                                                                                                    
	}    
	
	if (params.displayDebugInfo) printf("NumSubmeshes: %1d\n",stridingMeshInterface->getNumSubParts());
	const bool forceDegenerateTrianglesRemovalWhenACleanCopyOfTheMeshMustBeProcessed = true;    // Yes, much better
	if (!params.keepSubmeshesSeparated)    {
		std::vector< HACD::Vec3<HACD::Real> > points;
		std::vector< HACD::Vec3<long> > triangles;    
		//GetStridingInterfaceContent(stridingMeshInterface,points,triangles,-1,decomposeOnlySelectedSubmeshes.size()>0 ? &decomposeOnlySelectedSubmeshes : NULL);    // No support for keeping sub meshes separate ATM...
		GetStridingInterfaceContent(stridingMeshInterface,points,triangles,decomposeOnlySelectedSubmeshes.size()>0 ? &decomposeOnlySelectedSubmeshes : NULL);    // No support for keeping sub meshes separate ATM...        
		if (params.displayDebugInfo) printf("Numverts: %1d NumTriangles: %1d (from btStridingMeshInterface)\n",(int) points.size(),(int)triangles.size());    
		if (mustScaleMesh) ScalePoints(points,meshScalingFactor);
		if (params.decomposeACenteredCopyOfTheMesh) ShiftPoints(points,-aabbCenterPoint);
		
		if (params.decomposeADecimatedCopyOfTheMesh || params.decomposeACleanCopyOfTheMesh)    {
			const bool removeDegenerateTrianglesToo = params.decomposeADecimatedCopyOfTheMesh | forceDegenerateTrianglesRemovalWhenACleanCopyOfTheMeshMustBeProcessed; // Usually when removing double vertices, degenerate triangles are less prone to appear...
			RemoveDoubleVertices(points,triangles,removeDegenerateTrianglesToo,decimationDistance);    // This same method is used to decimate the mesh too.
			if (params.displayDebugInfo)    {
				if (params.decomposeADecimatedCopyOfTheMesh) printf("Numverts: %1d NumTriangles: %1d (after mesh decimation)\n",(int) points.size(),(int)triangles.size());    
				else if (params.decomposeACleanCopyOfTheMesh)  printf("Numverts: %1d NumTriangles: %1d (after duplicated vertices removal)\n",(int) points.size(),(int)triangles.size());    
			}    
		}
		
		HACD::HACD myHACD;
		myHACD.SetPoints(&points[0]);
		myHACD.SetNPoints(points.size());
		myHACD.SetTriangles(&triangles[0]);
		myHACD.SetNTriangles(triangles.size());
		
		myHACD.SetCompacityWeight(params.compacityWeight);
		myHACD.SetVolumeWeight(params.volumeWeight);
		myHACD.SetNClusters(params.nClusters);                     
		myHACD.SetNVerticesPerCH(params.maxHullVertices);                      
		myHACD.SetConcavity(params.concavity);                    
		myHACD.SetAddExtraDistPoints(params.addExtraDistPoints);   
		myHACD.SetAddNeighboursDistPoints(params.addNeighboursDistPoints);   
		myHACD.SetAddFacesPoints(params.addFacesPoints); 
		myHACD.SetConnectDist(params.connectionDistance);
		if (params.displayDebugInfo) myHACD.SetCallBack(&btHACDCompoundShape::HACDCallBackFunction);
		myHACD.SetScaleFactor(params.scaleFactor);
		
		myHACD.Compute(params.maxHullVertices == 0 ? true : false);
		
		if (params.optionalVRMLSaveFilePath.size()>0) myHACD.Save(params.optionalVRMLSaveFilePath.c_str(), false);
		
		for (size_t CH = 0,nClusters = myHACD.GetNClusters(); CH < nClusters; ++CH)    {
			
			const size_t nPoints =  myHACD.GetNPointsCH(CH);
			const size_t nTriangles =  myHACD.GetNTrianglesCH(CH);
			
			std::vector < HACD::Vec3<HACD::Real> > points(nPoints);
			std::vector < HACD::Vec3<long> > triangles(nTriangles);    // unused, but they could be used to display the convex hull...
			if (myHACD.GetCH(CH, &points[0], &triangles[0]))    {
				btConvexHullShape* convexHullShape = NULL;
				
				//calculate centroid
				btVector3 centroid(0,0,0);
				btVector3 minValue,maxValue,tempVert;
				if (nPoints>0)    {
					minValue=maxValue=btVector3(points[0].X(),points[0].Y(),points[0].Z());
				}
				else minValue=maxValue=btVector3(0,0,0);
				for (size_t i=1; i<nPoints; i++)    {
					tempVert = btVector3(points[i].X(),points[i].Y(),points[i].Z());
					minValue.setMin(tempVert);
					maxValue.setMax(tempVert);
				}
				centroid = (minValue+maxValue)*btScalar(0.5);
				
				//calculate convex hull shape
				btAlignedObjectArray < btVector3 > verts;
				verts.resize(nPoints);
				for (size_t i=0; i<nPoints; i++)    {
					verts[i]=btVector3(points[i].X()-centroid.x(),points[i].Y()-centroid.y(),points[i].Z()-centroid.z());
				}    
				if (params.shrinkObjectInwardsToCompensateCollisionMargin)    {
					btAlignedObjectArray<btVector3> planeEquations;
					btGeometryUtil::getPlaneEquationsFromVertices(verts,planeEquations);
					
					btAlignedObjectArray<btVector3> shiftedPlaneEquations;
					btVector3 plane;
					for (int p=0,psz = planeEquations.size();p<psz;p++)
					{
						plane = planeEquations[p];
						plane[3] += params.convexHullsCollisionMargin;
						shiftedPlaneEquations.push_back(plane);
					}
					btAlignedObjectArray<btVector3> shiftedVertices;
					btGeometryUtil::getVerticesFromPlaneEquations(shiftedPlaneEquations,shiftedVertices);
					
					convexHullShape = new btConvexHullShape(&(shiftedVertices[0].getX()),shiftedVertices.size());                
				}                            
				else convexHullShape = new btConvexHullShape(&verts[0].x(),verts.size());
				convexHullShape->setMargin(params.convexHullsCollisionMargin);
				if (params.convexHullsEnablePolyhedralContactClipping) convexHullShape->initializePolyhedralFeatures();    
				
				this->addChildShape(btTransform(btQuaternion::getIdentity(),centroid),convexHullShape);                
			}
		}    
	}    
	else {
		const int subParts = stridingMeshInterface->getNumSubParts();
		int curSelSm = -1;
		int curSelSmIdx = 0;    
		const bool mustSelectSubMeshes = (decomposeOnlySelectedSubmeshes.size()>0);            
		if (mustSelectSubMeshes) curSelSm = decomposeOnlySelectedSubmeshes[curSelSmIdx];
		std::vector< HACD::Vec3<HACD::Real> > points;
		std::vector< HACD::Vec3<long> > triangles;            
		for (int subPart = 0;subPart<subParts;subPart++)    {
			if (mustSelectSubMeshes)    {
				if (curSelSm!=subPart) continue;
				if (decomposeOnlySelectedSubmeshes.size()<=curSelSmIdx+1) curSelSm = subParts;    // This will never process successive cycles
				else curSelSm =  decomposeOnlySelectedSubmeshes[++curSelSmIdx];                    // Ready for next cycle    
			}
			//=====================================================================
			//region This code is almost copy and pasted from above (Better refactoring is needed...)
			//=====================================================================
			points.clear();
			triangles.clear();
			GetStridingInterfaceContentOfSingleSubmesh(stridingMeshInterface,points,triangles,subPart);
			//GetStridingInterfaceContent(stridingMeshInterface,points,triangles,subPart,NULL);
			
			if (params.displayDebugInfo) {
				printf("Subpart: %1d. Numverts: %1d NumTriangles: %1d (From btStridingMeshInterface)\n",subPart,(int) points.size(),(int)triangles.size());    
			}
			
			if (mustScaleMesh) ScalePoints(points,meshScalingFactor);
			if (params.decomposeACenteredCopyOfTheMesh) ShiftPoints(points,-aabbCenterPoint);
			
			
			if (params.decomposeADecimatedCopyOfTheMesh || params.decomposeACleanCopyOfTheMesh)    {
				const bool removeDegenerateTrianglesToo = params.decomposeADecimatedCopyOfTheMesh | forceDegenerateTrianglesRemovalWhenACleanCopyOfTheMeshMustBeProcessed; // Usually when removing double vertices, degenerate triangles are less prone to appear...
				RemoveDoubleVertices(points,triangles,removeDegenerateTrianglesToo,decimationDistance);    // This same method is used to decimate the mesh too.
				if (params.displayDebugInfo)    {
					if (params.decomposeADecimatedCopyOfTheMesh) printf("Subpart: %1d. Numverts: %1d NumTriangles: %1d (after mesh decimation)\n",subPart,(int) points.size(),(int)triangles.size());    
					else if (params.decomposeACleanCopyOfTheMesh)  printf("Subpart: %1d. Numverts: %1d NumTriangles: %1d (after duplicated vertices removal)\n",subPart,(int) points.size(),(int)triangles.size());    
				}
			}
			
			HACD::HACD myHACD;
			myHACD.SetPoints(&points[0]);
			myHACD.SetNPoints(points.size());
			myHACD.SetTriangles(&triangles[0]);
			myHACD.SetNTriangles(triangles.size());
			
			myHACD.SetCompacityWeight(params.compacityWeight);
			myHACD.SetVolumeWeight(params.volumeWeight);
			myHACD.SetNClusters(params.nClusters);                     
			myHACD.SetNVerticesPerCH(params.maxHullVertices);                      
			myHACD.SetConcavity(params.concavity);                    
			myHACD.SetAddExtraDistPoints(params.addExtraDistPoints);   
			myHACD.SetAddNeighboursDistPoints(params.addNeighboursDistPoints);   
			myHACD.SetAddFacesPoints(params.addFacesPoints); 
			myHACD.SetConnectDist(params.connectionDistance);
			if (params.displayDebugInfo) myHACD.SetCallBack(&btHACDCompoundShape::HACDCallBackFunction);
			myHACD.SetScaleFactor(params.scaleFactor);
			
			myHACD.Compute(params.maxHullVertices == 0 ? true : false);
			
			//if (params.optionalVRMLSaveFilePath.size()>0) myHACD.Save(params.optionalVRMLSaveFilePath.c_str(), false);
			
			for (size_t CH = 0,nClusters = myHACD.GetNClusters(); CH < nClusters; ++CH)    {
				
				const size_t nPoints =  myHACD.GetNPointsCH(CH);
				const size_t nTriangles =  myHACD.GetNTrianglesCH(CH);
				
				std::vector < HACD::Vec3<HACD::Real> > points(nPoints);
				std::vector < HACD::Vec3<long> > triangles(nTriangles); // unused, but they could be used to display the convex hull...
				if (myHACD.GetCH(CH, &points[0], &triangles[0]))    {
					btConvexHullShape* convexHullShape = NULL;
					
					//calculate centroid
					btVector3 centroid(0,0,0);
					btVector3 minValue,maxValue,tempVert;
					if (nPoints>0)    {
						minValue=maxValue=btVector3(points[0].X(),points[0].Y(),points[0].Z());
					}
					else minValue=maxValue=btVector3(0,0,0);
					for (size_t i=1; i<nPoints; i++)    {
						tempVert = btVector3(points[i].X(),points[i].Y(),points[i].Z());
						minValue.setMin(tempVert);
						maxValue.setMax(tempVert);
					}
					centroid = (minValue+maxValue)*btScalar(0.5);
					
					//calculate convex hull shape
					btAlignedObjectArray < btVector3 > verts;
					verts.resize(nPoints);
					for (size_t i=0; i<nPoints; i++)    {
						verts[i]=btVector3(points[i].X()-centroid.x(),points[i].Y()-centroid.y(),points[i].Z()-centroid.z());
					}    
					if (params.shrinkObjectInwardsToCompensateCollisionMargin)    {
						btAlignedObjectArray<btVector3> planeEquations;
						btGeometryUtil::getPlaneEquationsFromVertices(verts,planeEquations);
						
						btAlignedObjectArray<btVector3> shiftedPlaneEquations;
						btVector3 plane;
						for (int p=0,psz = planeEquations.size();p<psz;p++)
						{
							plane = planeEquations[p];
							plane[3] += params.convexHullsCollisionMargin;
							shiftedPlaneEquations.push_back(plane);
						}
						btAlignedObjectArray<btVector3> shiftedVertices;
						btGeometryUtil::getVerticesFromPlaneEquations(shiftedPlaneEquations,shiftedVertices);
						
						convexHullShape = new btConvexHullShape(&(shiftedVertices[0].getX()),shiftedVertices.size());                
					}                            
					else convexHullShape = new btConvexHullShape(&verts[0].x(),verts.size());
					convexHullShape->setMargin(params.convexHullsCollisionMargin);
					if (params.convexHullsEnablePolyhedralContactClipping) convexHullShape->initializePolyhedralFeatures();    
					
					this->addChildShape(btTransform(btQuaternion::getIdentity(),centroid),convexHullShape);    
					m_submeshIndexOfChildShapes.push_back(subPart);            
				}
			}                
			//=====================================================================
			//endregion
			//=====================================================================
		}
		
		for (int i=0,sz=m_submeshIndexOfChildShapes.size();i<sz;i++)    {
			printf("submeshIndexOfChildShapes[%1d]    =    %1d;\n",i,m_submeshIndexOfChildShapes[i]);        
		}
	}
	
	if (params.optionalBCSSaveFilePath.size()>0 && this->getNumChildShapes()>0)    {
		btDefaultSerializer serializer;
		serializer.startSerialization();
		this->serializeSingleShape(&serializer);
		serializer.finishSerialization();      
		FILE* file = fopen(params.optionalBCSSaveFilePath.c_str(),"wb");
		if (file)    {
			fwrite(serializer.getBufferPointer(),serializer.getCurrentBufferSize(),1, file);
			fclose(file);
			if (params.displayDebugInfo) printf("btCompoundShape saved to: \"%s\"\n",params.optionalBCSSaveFilePath.c_str());
		}
		else {    
			printf("ERROR: I can't save the btCompoundShape to: \"%s\"\n",params.optionalBCSSaveFilePath.c_str());
		}
		
	}
	//const std::clock_t ck2 = std::clock();
	//if (params.displayDebugInfo) printf("Operation performed in %1.2f seconds.\n",((float)(ck2-ck1))*0.001);
}

void btHACDCompoundShape::GetStridingInterfaceContentOfSingleSubmesh(const btStridingMeshInterface* sti,std::vector< HACD::Vec3<HACD::Real> >& vertsOut,std::vector< HACD::Vec3<long> >& trianglesOut,int subMeshIndex) {
	typedef HACD::Real vertexType;
	typedef long indexType;
	
	vertsOut.clear();trianglesOut.clear();
	if (!sti) return;
	
	const int subparts = sti->getNumSubParts();
	if (subparts<=0 || subMeshIndex<0 || subMeshIndex>=subparts) return;        
	
	const unsigned char *pverts;int numVerts;    PHY_ScalarType type;int stride;
	const unsigned char *pinds;int indsStride;int numTriangles;PHY_ScalarType indsType;
	
	unsigned int vCnt=0;
	unsigned int tCnt=0;
	
	const int subpart = subMeshIndex;
	
	{
		sti->getLockedReadOnlyVertexIndexBase(&pverts,numVerts,type,stride,&pinds,indsStride,numTriangles,indsType,subpart);
		const size_t startVerts = vertsOut.size();
		const size_t startTriangles = trianglesOut.size();
		
		try    {
			
			
			switch (type)    {
				case PHY_FLOAT:    {
					typedef float glVertex;
					const glVertex* verts = (const glVertex*) pverts; 
					const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
					
					vertsOut.resize(startVerts+numVerts);
					for (int i = 0; i < numVerts;++i)    {
						vertsOut[vCnt].X()=(vertexType) *verts++;
						vertsOut[vCnt].Y()=(vertexType) *verts++;
						vertsOut[vCnt++].Z()=(vertexType) *verts++;
						
						verts+=vertexDeltaStrideInGLVertexUnits;
					}                    
					break;
				}
				case PHY_DOUBLE:{
					typedef double glVertex;
					const glVertex* verts = (const glVertex*) pverts; 
					const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
					
					vertsOut.resize(startVerts+numVerts);
					for (int i = 0; i < numVerts;++i)    {
						vertsOut[vCnt].X()=(vertexType) *verts++;
						vertsOut[vCnt].Y()=(vertexType) *verts++;
						vertsOut[vCnt++].Z()=(vertexType) *verts++;
						
						verts+=vertexDeltaStrideInGLVertexUnits;
					}
					break;
				}
				default:
					btAssert((type == PHY_FLOAT) || (type == PHY_DOUBLE));
			}
			
			
			switch (indsType)    {
				case PHY_INTEGER:    {
					typedef int glIndex;
					const glIndex* inds = (const glIndex*) pinds; 
					const int indexDeltaStrideInGLIndexUnits = indsStride <= (int)(sizeof(indsStride)*3 ? 0 : (stride-sizeof(indsStride)*3)/sizeof(indsStride)); 
					
					trianglesOut.resize(startTriangles+numTriangles);
					for (int i = 0; i < numTriangles;++i)    {
						trianglesOut[tCnt].X() = (indexType) *inds++;
						trianglesOut[tCnt].Y() = (indexType) *inds++;
						trianglesOut[tCnt++].Z() = (indexType) *inds++;
						
						inds+=indexDeltaStrideInGLIndexUnits;
					}
					break;
				}
				case PHY_SHORT:    {
					typedef unsigned short glIndex;
					const glIndex* inds = (const glIndex*) pinds;
					const int indexDeltaStrideInGLIndexUnits = indsStride <= (int)(sizeof(indsStride)*3 ? 0 : (stride-sizeof(indsStride)*3)/sizeof(indsStride));                      
					
					trianglesOut.resize(startTriangles+numTriangles);
					for (int i = 0; i < numTriangles;++i)    {
						trianglesOut[tCnt].X() = (indexType) *inds++;
						trianglesOut[tCnt].Y() = (indexType) *inds++;
						trianglesOut[tCnt++].Z() = (indexType) *inds++;
						
						inds+=indexDeltaStrideInGLIndexUnits;                                                
					}                              
					break;
				}
				default:
					btAssert((indsType == PHY_INTEGER) || (indsType == PHY_SHORT));
			}
		}
		catch (...) {
			vertsOut.resize(startVerts);
			trianglesOut.resize(startTriangles);
		}
		
		sti->unLockReadOnlyVertexBase(subpart);
	}
}
void btHACDCompoundShape::GetStridingInterfaceContent(const btStridingMeshInterface* sti,std::vector< HACD::Vec3<HACD::Real> >& vertsOut,std::vector< HACD::Vec3<long> >& trianglesOut,const btAlignedObjectArray< int >* processOnlySelectedSubmeshesInAscendingOrder) {
	typedef HACD::Real vertexType;
	typedef long indexType;
	
	vertsOut.clear();trianglesOut.clear();
	if (!sti) return;
	
	const int subparts = sti->getNumSubParts();
	if (subparts<=0 || (processOnlySelectedSubmeshesInAscendingOrder && processOnlySelectedSubmeshesInAscendingOrder->size()==0)) return;        
	
	const unsigned char *pverts;int numVerts;    PHY_ScalarType type;int stride;
	const unsigned char *pinds;int indsStride;int numTriangles;PHY_ScalarType indsType;
	
	btAlignedObjectArray < const unsigned char * > vertexBaseOfProcessedSubMeshes;
	btAlignedObjectArray < int > startVertexIndexOfVertexBaseOfProcessedSubMeshes;
	//#define DONT_REUSE_SHARED_VERTEX_ARRAY    // problably safer, but much slower
	
	unsigned int vCnt=0;
	unsigned int tCnt=0;
	
	int curSelSm = -1;
	int curSelSmIdx = 0;    
	//int numProcessedSubmeshes = 0;
	const bool mustSelectSubMeshes = processOnlySelectedSubmeshesInAscendingOrder;            
	if (mustSelectSubMeshes) curSelSm = (*processOnlySelectedSubmeshesInAscendingOrder)[curSelSmIdx];
	
	//::MessageBox(NULL,sw::ToString(subParts).c_str(),_T("subParts"),MB_OK);    
	
	for (int subpart=0;subpart<subparts;subpart++)    {    
		//-----------------------------------------------------------------------------------------------------------------------------------
		if (curSelSm>=0 && curSelSm!=subpart) continue;
		else if (mustSelectSubMeshes) {
			// Prepare the index for next cycle and continue:
			if (processOnlySelectedSubmeshesInAscendingOrder->size()<=curSelSmIdx+1) curSelSm = subparts;    // This will never process successive cycles
			else curSelSm =  (*processOnlySelectedSubmeshesInAscendingOrder)[++curSelSmIdx];                // Ready for next cycle            
		}
		//------------------------------------------------------------------------------------------------------------------------------------                
		sti->getLockedReadOnlyVertexIndexBase(&pverts,numVerts,type,stride,&pinds,indsStride,numTriangles,indsType,subpart);
		const size_t startVerts = vertsOut.size();
		const size_t startTriangles = trianglesOut.size();
		
		int startVertexIndex = startVerts;
		bool vertexArrayIsShared = false;    
		
#ifndef DONT_REUSE_SHARED_VERTEX_ARRAY    
		{
			for (int i = 0,sz = vertexBaseOfProcessedSubMeshes.size();i<sz;i++)    {
				if (pverts == vertexBaseOfProcessedSubMeshes[i])    {
					startVertexIndex = startVertexIndexOfVertexBaseOfProcessedSubMeshes[i];
					vertexArrayIsShared = true;
					break;
				}
			} 
			
			vertexBaseOfProcessedSubMeshes.push_back(pverts);
			startVertexIndexOfVertexBaseOfProcessedSubMeshes.push_back(startVertexIndex);
		}
#else
#undef DONT_REUSE_SHARED_VERTEX_ARRAY
#endif //DONT_REUSE_SHARED_VERTEX_ARRAY
		
		//const indexType indsAdder = (indexType) startVertexIndex; 
		
		
		try    {
			
			if (!vertexArrayIsShared)                
				switch (type)    {
					case PHY_FLOAT:    {
						typedef float glVertex;
						const glVertex* verts = (const glVertex*) pverts; 
						const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
						
						vertsOut.resize(startVerts+numVerts);
						for (int i = 0; i < numVerts;++i)    {
							vertsOut[vCnt].X()=(vertexType) *verts++;
							vertsOut[vCnt].Y()=(vertexType) *verts++;
							vertsOut[vCnt++].Z()=(vertexType) *verts++;
							
							verts+=vertexDeltaStrideInGLVertexUnits;
						}                    
						break;
					}
					case PHY_DOUBLE:{
						typedef double glVertex;
						const glVertex* verts = (const glVertex*) pverts; 
						const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
						
						vertsOut.resize(startVerts+numVerts);
						for (int i = 0; i < numVerts;++i)    {
							vertsOut[vCnt].X()=(vertexType) *verts++;
							vertsOut[vCnt].Y()=(vertexType) *verts++;
							vertsOut[vCnt++].Z()=(vertexType) *verts++;
							
							verts+=vertexDeltaStrideInGLVertexUnits;
						}
						break;
					}
					default:
						btAssert((type == PHY_FLOAT) || (type == PHY_DOUBLE));
				}
			
			
			switch (indsType)    {                
				case PHY_INTEGER:    {
					typedef int glIndex;
					const glIndex* inds = (const glIndex*) pinds; 
					const int indexDeltaStrideInGLIndexUnits = indsStride <= (int)(sizeof(indsStride)*3 ? 0 : (stride-sizeof(indsStride)*3)/sizeof(indsStride)); 
					
					trianglesOut.resize(startTriangles+numTriangles);
					for (int i = 0; i < numTriangles;++i)    {
						trianglesOut[tCnt].X() = startVertexIndex + (indexType) *inds++;
						trianglesOut[tCnt].Y() = startVertexIndex + (indexType) *inds++;
						trianglesOut[tCnt++].Z() = startVertexIndex + (indexType) *inds++;
						
						inds+=indexDeltaStrideInGLIndexUnits;
					}
					break;
				}
				case PHY_SHORT:    {
					typedef unsigned short glIndex;
					const glIndex* inds = (const glIndex*) pinds;
					const int indexDeltaStrideInGLIndexUnits = indsStride <= (int)(sizeof(indsStride)*3 ? 0 : (stride-sizeof(indsStride)*3)/sizeof(indsStride));                      
					
					trianglesOut.resize(startTriangles+numTriangles);
					for (int i = 0; i < numTriangles;++i)    {
						trianglesOut[tCnt].X() = startVertexIndex + (indexType) *inds++;
						trianglesOut[tCnt].Y() = startVertexIndex + (indexType) *inds++;
						trianglesOut[tCnt++].Z() = startVertexIndex + (indexType) *inds++;
						
						inds+=indexDeltaStrideInGLIndexUnits;                                                
					}                              
					break;
				}
				default:
					btAssert((indsType == PHY_INTEGER) || (indsType == PHY_SHORT));
			}
		}
		catch (...) {
			vertsOut.resize(startVerts);
			trianglesOut.resize(startTriangles);
		}
		
		sti->unLockReadOnlyVertexBase(subpart);
	}
}
HACD::Vec3<HACD::Real> btHACDCompoundShape::GetAabbHalfExtentsFromStridingInterface(const btStridingMeshInterface* sti,HACD::Vec3<HACD::Real>* pOptionalCenterPointOut,const HACD::Vec3<HACD::Real>& scalingFactorToApply) {
	typedef HACD::Real vertexType;
	
	HACD::Vec3<vertexType> aabbHalfExtents(0,0,0);
	if (pOptionalCenterPointOut) *pOptionalCenterPointOut = HACD::Vec3<HACD::Real>(0,0,0);
	
	HACD::Vec3<vertexType> cmax(0,0,0);
	HACD::Vec3<vertexType> cmin(0,0,0);
	bool noStartingVertex = true;
	HACD::Vec3<vertexType> temp;
	
	if (!sti) return aabbHalfExtents;
	
	const int subParts = sti->getNumSubParts();
	const unsigned char *pverts;int numVerts;PHY_ScalarType type;int stride;
	const unsigned char *pinds;int indsStride;int numTriangles;PHY_ScalarType indsType;    // Unused, but needed
	
	for (int subpart=0;subpart<subParts;subpart++)    {     
		sti->getLockedReadOnlyVertexIndexBase(&pverts,numVerts,type,stride,&pinds,indsStride,numTriangles,indsType,subpart);
		try    {                        
			switch (type)    {
				case PHY_FLOAT:    {
					typedef float glVertex;
					const glVertex* verts = (const glVertex*) pverts; 
					const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
					
					for (int i = 0; i < numVerts;++i)    {                        
						temp.X()=(vertexType) *verts++;
						temp.Y()=(vertexType) *verts++;
						temp.Z()=(vertexType) *verts++;                        
						verts+=vertexDeltaStrideInGLVertexUnits;
						
						if (noStartingVertex)    {
							noStartingVertex = false;
							cmin = temp;cmax = temp;
						}
						else    {
							if         (cmax.X()<temp.X()) cmax.X()=temp.X();
							else if (cmin.X()>temp.X())    cmin.X()=temp.X();
							if         (cmax.Y()<temp.Y()) cmax.Y()=temp.Y();
							else if (cmin.Y()>temp.Y())    cmin.Y()=temp.Y();
							if         (cmax.Z()<temp.Z()) cmax.Z()=temp.Z();
							else if (cmin.Z()>temp.Z())    cmin.Z()=temp.Z();                                                        
						}
					}                    
					break;
				}
				case PHY_DOUBLE:{
					typedef double glVertex;
					const glVertex* verts = (const glVertex*) pverts; 
					const int vertexDeltaStrideInGLVertexUnits = stride <= (int)(sizeof(glVertex)*3 ? 0 : (stride-sizeof(glVertex)*3)/sizeof(glVertex));             
					
					for (int i = 0; i < numVerts;++i)    {
						temp.X()=(vertexType) *verts++;
						temp.Y()=(vertexType) *verts++;
						temp.Z()=(vertexType) *verts++;                        
						verts+=vertexDeltaStrideInGLVertexUnits;
						
						if (noStartingVertex)    {
							noStartingVertex = false;
							cmin = temp;cmax = temp;
						}
						else    {
							if         (cmax.X()<temp.X()) cmax.X()=temp.X();
							else if (cmin.X()>temp.X())    cmin.X()=temp.X();
							if         (cmax.Y()<temp.Y()) cmax.Y()=temp.Y();
							else if (cmin.Y()>temp.Y())    cmin.Y()=temp.Y();
							if         (cmax.Z()<temp.Z()) cmax.Z()=temp.Z();
							else if (cmin.Z()>temp.Z())    cmin.Z()=temp.Z();                                                        
						}
					}
					break;
				}
				default:
					btAssert((type == PHY_FLOAT) || (type == PHY_DOUBLE));
			}
		}
		catch (...) {
		}
		
		sti->unLockReadOnlyVertexBase(subpart);
	}    
	
	aabbHalfExtents = (cmax - cmin)*vertexType(0.5);
	aabbHalfExtents = HACD::Vec3<HACD::Real>(aabbHalfExtents.X()*scalingFactorToApply.X(),aabbHalfExtents.Y()*scalingFactorToApply.Y(),aabbHalfExtents.Z()*scalingFactorToApply.Z());
	if (pOptionalCenterPointOut) {
		*pOptionalCenterPointOut =  (cmax + cmin)*vertexType(0.5);
		*pOptionalCenterPointOut = HACD::Vec3<HACD::Real>(pOptionalCenterPointOut->X()*scalingFactorToApply.X(),pOptionalCenterPointOut->Y()*scalingFactorToApply.Y(),pOptionalCenterPointOut->Z()*scalingFactorToApply.Z()); 
	}    
	return aabbHalfExtents;
}

void btHACDCompoundShape::RemoveDoubleVertices(std::vector< HACD::Vec3<HACD::Real> >& verts,std::vector< HACD::Vec3<long> >& triangles,const bool removeDegenerateTrianglesToo, const HACD::Vec3<HACD::Real>& eps) {
	typedef long U;
	typedef HACD::Vec3<HACD::Real> Vector3;
	
	U vertsSize = (U) verts.size();
	size_t trianglesSize = triangles.size();
	
	std::vector<U> S(vertsSize,vertsSize);    
	
	U cnt = 0, minDoubleVert = vertsSize, maxSingleVert = 0;
	
	std::vector< Vector3 > newVerts;                             
	newVerts.reserve(vertsSize);
	
	//---------- Optional Stuff--------------------
	// Basically we want to average the double verts before removing them
	// We can set to false the following line to speed up code...
	const bool useFineTuningForVerts = true;
	const bool useSomeFineTuning = useFineTuningForVerts;
	
	std::vector < unsigned int > C;    // counts the number a given vert is repeated
	if (useSomeFineTuning) C.reserve(vertsSize);
	
	//----------------------------------------------
	for (U t=0;t<vertsSize;t++)    {
		if (S[t]!=vertsSize) continue;
		S[t]=cnt;
		maxSingleVert = t;
		const Vector3* vertsT = &verts[t];
		if (useSomeFineTuning) C.push_back(1);    
		newVerts.push_back(*vertsT);
		for (U u=t+1;u<vertsSize;u++)    {
			if (AreEqual(*vertsT,verts[u],eps))    {
				S[u] = cnt;
				if (minDoubleVert > u) minDoubleVert = u;
				if (useSomeFineTuning)    {
					C[cnt]+=1;
					if (useFineTuningForVerts) newVerts[cnt]+=verts[u];    //These verts should be equal... however if we pass a bigger "eps" they could differ...
				}
			}
		}
		cnt++;
	} 
	
	const size_t newVertsSize = cnt;
	if (useSomeFineTuning)    {    // We must average the data so that we can replace multiple equal vertices with their average value
		for (size_t i = 0;i<newVertsSize;i++)    {
			if (C[i]==1) continue;
			const float count = (float) C[i];
			if (useFineTuningForVerts) newVerts[i]/=count;        
		}
	}
	
	
	{
		std::vector< HACD::Vec3<U> > newTriangles(trianglesSize);
		size_t numDegenerateTriangles = 0;    // Never used
		size_t cnt = 0;
		for (size_t t=0;t<trianglesSize;t++)    {
			const HACD::Vec3<U>& tri = triangles[t];
			HACD::Vec3<U>& newTri = newTriangles[cnt];
			newTri.X() = S[tri.X()];
			newTri.Y() = S[tri.Y()];
			newTri.Z() = S[tri.Z()];
			if (removeDegenerateTrianglesToo && (newTri.X()==newTri.Y() || newTri.Y()==newTri.Z() || newTri.X()==newTri.Z())) {
				++numDegenerateTriangles;continue;
			}
			++cnt;
		}
		if (trianglesSize!=cnt) newTriangles.resize(cnt);
		triangles = newTriangles;
	}
	
	
	verts = newVerts;    
	
}
