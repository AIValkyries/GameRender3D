#pragma once
#ifndef OgreMeshFileFormat_H
#define OgreMeshFileFormat_H

/**
*  A chunk always consists of:
*    unsigned short CHUNK_ID        : one of the following chunk ids identifying the chunk
*    unsigned long  LENGTH          : length of the chunk in bytes, including this header
*    void*          DATA            : the data, which may contain other sub-chunks (various data types)
*/

/**
struct test
{
	// uint16* id  SwapEndian  0x1000 == false 0x0010 == true;
    // char*          version           : Version number check
	// bool skeletallyAnimated   // important flag which affects h/w buffer policies
	// Optional M_GEOMETRY chunk
	Mesh
	{
		// char* materialName
		// bool useSharedVertices
		// unsigned int indexCount
		// bool indexes32Bit
		// unsigned int* faceVertexIndices (indexCount)
		// OR
		// unsigned short* faceVertexIndices (indexCount)
		SubMesh, OGRE_SUBMESH
		{
			// unsigned short operationType
			Submesh_Operation = OGRE_SUBMESH_OPERATION
			// Optional bone weights (repeating section)
		    // unsigned int vertexIndex;
		    // unsigned short boneIndex;
		    //float32 weight
			Submesh_Bone_assignment = OGRE_SUBMESH_BONE_ASSIGNMENT

			// char* aliasName;
			// char* textureName;
		    SubMesh_Texture_alias = OGRE_SUBMESH_TEXTURE_ALIAS

			// unsigned int vertexCount
		    Geometry,= OGRE_GEOMETRY
		    {
			   Geometry_Vertex_Declaration = OGRE_GEOMETRY_VERTEX_DECLARATION
			   {
				  // unsigned short source;
				  // unsigned short type;
				  // unsigned short semantic;
				  // unsigned short offset;
				  // unsigned short index;
				  Geometry_Vertex_Element = OGRE_GEOMETRY_VERTEX_ELEMENT
			  }
			  // unsigned short vertexSize;
			  // unsigned short vertexSize;
			  Geometry_Vertex_Buffer = OGRE_GEOMETRY_VERTEX_BUFFER
			  {
				  // raw buffer data
				  Geometry_Vertex_Buffer_Data = OGRE_GEOMETRY_VERTEX_BUFFER_DATA
			  }
		   }
	    }

		// char* skeletonName           : name of .skeleton to use
		//Optional link to skeleton
		Mesh_Skeleton_link, OGRE_MESH_SKELETON_LINK

		// Optional bone weights (repeating section)
		// unsigned int vertexIndex;
		// unsigned short boneIndex;
		// float weight;
		Mesh_Bone_Assignment,OGRE_MESH_BONE_ASSIGNMENT

		// Optional LOD information
		// string strategyName;
		// unsigned short numLevels;
		// bool manual;  
		Mesh_Lod_Level = OGRE_MESH_LOD_LEVEL
		{
			// float lodValue;
			Mesh_Lod_Usage
		    {
			    // String manualMeshName;
			    Mesh_Lod_Manual,
				Mesh_Lod_Generated,
	        }
	    }

		// float minx, miny, minz
		// float maxx, maxy, maxz
		// float radius
		Mesh_Bounds = OGRE_MESH_BOUNDS

		Mesh_Name_Table,= OGRE_SUBMESH_NAME_TABLE
		{
			Submesh_Name_Table_Element
	    }

		EDGE_Lists=OGRE_EDGE_LISTS
		{
			Edge_List_Lod
		    {
			    Edge_Group
	        }
	    }

		Poses=OGRE_POSES
		{
			Pose,
		    {
			   Pose_Vertex
	        }
	    }

		//Optional vertex animation chunk
		Animations,=OGRE_ANIMATIONS
		{
			// char* name
			// float length
			Animation

			// char* baseAnimationName (blank for self)
			// float baseKeyFrameTime
			Animation_Baseinfo

			// unsigned short type
			// unsigned short target  
			// 1+ for submesh index + 1
		    Animation_Track
			{
				// float time
				// bool includesNormals [1.8+]
				// float x,y,z    
				Animation_Morph_KeyFrame
				// float time
				Animation_Pose_KeyFrame
		        {
				   // float influence
				   // unsigned short poseIndex 
			       Animation_Pose_Ref
	            }
	        }
	    }

		Table_Extremes
	}
};
*/

/**
* 数据结构
* {
*    |Bone|,|Bone|,|...|
* }
* {
*    |BoneParent|,|BoneParent|,|...|
* }
* {
*    |Animation|,|AnimationTrack|,|Keyframe|,|Animation|,|AnimationTrack|,|Keyframe|,|...|
* }
* 
struct Skelete
{
    id 0x1000 = false;  0x1010 = true
	// char* version 
	Header,
	{
	    // unsigned short blendmode
		blendmode
	};

	// char* name
	// unsigned short handle
	// Vector3 position
	// Quaternion orientation 
	// Vector3 scale
	Bone;

	// unsigned short handle             : child bone
	// unsigned short parentHandle   : parent bone
	bone_parent;

	// char* name
	// float length   
	animation
	{
		// [Optional] base keyframe information
		// char* baseAnimationName (blank for self)
		// float baseKeyFrameTime
		animation_baseinfo

		// unsigned short boneIndex
		animation_track
		{
			// float time
			// Quaternion rotate
			// Vector3 translate
			// Vector3 scale  
			animation_track_keyframe
		};
	};

	// char* skeletonName
	// float scale
	animation_link
};

*/

enum MESH_CHUNK_ID
{
	OGRE_HEADER = 0x1000,
	// char*          version           : Version number check
	OGRE_MESH = 0x3000,
	   // bool skeletallyAnimated 
	   // char* materialName
	   // bool useSharedVertices
	   // unsigned int indexCount
	   // unsigned int* faceVertexIndices(indexCount)
	   // OR
	   // unsigned short* faceVertexIndices(indexCount)
	   // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
	   OGRE_SUBMESH = 0x4000,
	     // optional, trilist assumed if missing
	     OGRE_SUBMESH_OPERATION = 0x4010,
		 // Optional bone weights  (repeating section)
		 // unsigned int vertexIndex;
		 // unsigned short boneIndex;
		 // float32 weight
		 OGRE_SUBMESH_BONE_ASSIGNMENT = 0x4100,
		 // char* aliasName;
		 // char* textureName
		 OGRE_SUBMESH_TEXTURE_ALIAS = 0x4200,

		 OGRE_GEOMETRY = 0x5000,
		     // unsigned short source;  // buffer bind source
			 // unsigned short type;     // VertexElementType
			 // unsigned short semantic; // VertexElementSemantic
			 // unsigned short offset;   // start offset in buffer in bytes
			 // unsigned short index;    // index of the semantic (for colours and texture coords)
			 OGRE_GEOMETRY_VERTEX_DECLARATION = 0x5100,
			     OGRE_GEOMETRY_VERTEX_ELEMENT = 0x5110,
			 // unsigned short bindIndex;  Index to bind this buffer to
			 // unsigned short vertexSize; 
			 OGRE_GEOMETRY_VERTEX_BUFFER = 0x5200,
				 //raw buffer data
				 OGRE_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
         OGRE_MESH_SKELETON_LINK = 0x6000,

		 // Optional bone weights (repeating section)
		 // unsigned int vertexIndex;
		 // unsigned short boneIndex;
		 // float32 weight;
		 OGRE_MESH_BONE_ASSIGNMENT = 0x7000,
		 
		 // Optional LOD information
         // string strategyName;
         // unsigned short numLevels;
         // bool manual;  (true for manual alternate meshes, false for generated)
		 OGRE_MESH_LOD_LEVEL = 0x8000,
		      //  NB LOD 0 (full detail from 0 depth) is omitted
			  // LOD value - this is a distance, a pixel count etc, based on strategy
			  // float32 lodValue;
			  OGRE_MESH_LOD_USAGE = 0x8100,
			      // String manualMeshName;
				  OGRE_MESH_LOD_MANUAL = 0x8110,
				  // unsigned int indexCount;
				  // bool indexes32Bit
				  // unsigned short* faceIndexes;  (indexCount)
				  // OR
				  // unsigned int* faceIndexes;  (indexCount)
				  OGRE_MESH_LOD_GENERATED = 0x8120,
		  // float32 minx, miny, minz
		  // float32 maxx, maxy, maxz
		  // float32 radius
		  OGRE_MESH_BOUNDS = 0x9000,

		  OGRE_SUBMESH_NAME_TABLE = 0xA000,
		       // short index
			   // char* name
			   OGRE_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,
		  OGRE_EDGE_LISTS = 0xB000,
		       // unsigned short lodIndex
			   // bool isManual 
			   // bool isClosed
			   // unsigned long numTriangles
			   // unsigned long numEdgeGroups
			   // Triangle* triangleList
			   // unsigned long indexSet
			   // unsigned long vertexSet
			   // unsigned long vertIndex[3]
			   // unsigned long sharedVertIndex[3] 
			   // float normal[4] 
			   OGRE_EDGE_LIST_LOD = 0xB100,
			        // unsigned long vertexSet
					// unsigned long triStart
					// unsigned long triCount
					// unsigned long numEdges
					// Edge* edgeList
					// unsigned long  triIndex[2]
					// unsigned long  vertIndex[2]
					// unsigned long  sharedVertIndex[2]
					// bool degenerate
					OGRE_EDGE_GROUP = 0xB110,
		   OGRE_POSES = 0xC000,
		        // char* name (may be blank)
				// unsigned short target
				// 0 for shared geometry, 
				// 1+ for submesh index + 1
				// bool includesNormals [1.8+]
				OGRE_POSE = 0xC100,
				    // unsigned long vertexIndex
				    // float xoffset, yoffset, zoffset
				    // float xnormal, ynormal, znormal (optional, 1.8+)
		            OGRE_POSE_VERTEX = 0xC111,
		   OGRE_ANIMATIONS = 0xD000,
		        // char* name
				// float length
		        OGRE_ANIMATION = 0xD100,
		        // char* baseAnimationName (blank for self)
				// float baseKeyFrameTime
		        OGRE_ANIMATION_BASEINFO = 0xD105,
		        // unsigned short type
			    // unsigned short target
			    OGRE_ANIMATION_TRACK = 0xD110,
			         //float time	
					 //bool includesNormals [1.8+]
					 //float x,y,z
			         OGRE_ANIMATION_MORPH_KEYFRAME = 0xD111,
			         // float time
			         OGRE_ANIMATION_POSE_KEYFRAME = 0xD112,
			         //unsigned short poseIndex 
					 //float influence
			         OGRE_ANIMATION_POSE_REF = 0xD113,
		OGRE_TABLE_EXTREMES = 0xE000
};


enum SKELETON_CHUNK_ID
{
	// char* version           : Version number check
	OGRE_SKELETON_HEADER = 0x1000,
	     // unsigned short blendmode     : SkeletonAnimationBlendMode
	     OGRE_SKELETON_BLENDMODE = 0x1010,
	// Repeating section defining each bone in the system. 
	// Bones are assigned indexes automatically based on their order of declaration
	// starting with 0.

	// char* name
	// unsigned short handle
	// Vector3 position
	// Quaternion orientation
	// Vector3 scale
	OGRE_SKELETON_BONE = 0x2000,
	// Record of the parent of a single bone, used to build the node tree
	// Repeating section, listed in Bone Index order, one per Bone

	// unsigned short handle             : child bone
	// unsigned short parentHandle   : parent bone
	OGRE_SKELETON_BONE_PARENT = 0x3000,

	// A single animation for this skeleton
	// char* name 
	// float32 length 
	OGRE_SKELETON_ANIMATION = 0x4000, 
	     // [Optional] base keyframe information 
		 // char* baseAnimationName (blank for self)
		 // float baseKeyFrameTime
		 OGRE_SKELETON_ANIMATION_BASEINFO = 0x4010,
		 // A single animation track (relates to a single bone)
		 // Repeating section (within SKELETON_ANIMATION)

		 // unsigned short boneIndex
		 OGRE_SKELETON_ANIMATION_TRACK = 0x4100,
		      // A single keyframe within the track
			  // Repeating section

			  // float time
			  // Quaternion rotate
			  // Vector3 translate
			  // Vector3 scale
			  OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME = 0x4110,
	 // Link to another skeleton, to re-use its animations
	 // char* skeletonName
	 // float scale
	 OGRE_SKELETON_ANIMATION_LINK = 0x5000
};


#endif
