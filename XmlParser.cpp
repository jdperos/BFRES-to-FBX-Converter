#include "XmlParser.h"

namespace XML
{


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::Parse(const char* filePath, BFRES& bfres)
    {
        File file(filePath);
        Document doc;
        ParseDocument(file, doc);

        Element* pRoot = doc.first_node();
        std::string token = "";

        // Parse FMDLs
        uint32 fmdlIndex = 0;
        Element* pNode = pRoot->first_node("FMDL");
        while (pNode)
        {
            FMDL fmdl;
            fmdl.index = fmdlIndex;
            ParseFMDL(fmdl, pNode);
            bfres.fmdl.push_back(fmdl);
            pNode = pNode->next_sibling("FMDL");
            fmdlIndex++;
        }

        //pNode = pNode->first_node("Shapes")->first_node();
        //while(pNode)
        //{
        //    if (!(pNode->next_sibling()))
        //        break;
        //    pNode = pNode->next_sibling();
        //}

        //pNode = pNode->first_node("Vertices")->first_node();
        //while(pNode)
        //{
        //    if (!(pNode->next_sibling()))
        //        break;
        //    pNode = pNode->next_sibling();
        //}

        //if (ParseAttributeString(token, pNode, "Index"))
        //    printf("Last Index is %s", token.c_str());
        //for (Attribute* attr = node->first_attribute();
        //    attr; attr = attr->next_attribute())
        //{
        //    std::cout << "Node foobar has attribute " << attr->name() << " ";
        //    std::cout << "with value " << attr->value() << "\n";
        //}
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseDocument(File& file, Document& doc)
    {
        doc.parse<0>(file.data());
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFMDL(FMDL& fmdl, Element* pElement)
    {
        std::string token = "";
        if (ParseAttributeString(token, pElement, "Name"))        fmdl.name = token;

        // Parse FSKL
        {
            Element* pNode = pElement->first_node("FSKL");
            ParseFSKL(fmdl.fskl, pNode);
        }

        // Parse Material
        {
            // TODO
            Element* pNode = pElement->first_node("Materials");
            ParseMaterials(fmdl.fmats, pNode);
        }

        // Parse Shapes
        {
            // TODO
            Element* pNode = pElement->first_node("Shapes");
            ParseShapes(fmdl.index, fmdl.fshps, pNode);
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFSKL(FSKL& fskl, Element* pElement)
    {
        std::string token = "";

        ParseAttributeUInt(fskl.boneCount, pElement, "SkeletonBoneCount");
        ParseAttributeUIntArray(fskl.boneList, pElement, "BoneList");

        // Parse Bones
        Element* pNode = pElement->first_node("Bone");
        while (pNode)
        {
            Bone bone;
            ParseBone(bone, pNode);
            fskl.bones.push_back(bone);
            pNode = pNode->next_sibling("Bone");
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseBone(Bone& bone, Element* pElement)
    {
        ParseAttributeString(bone.name              , pElement, "Name");
        ParseAttributeUInt(bone.index               , pElement, "Index");
        ParseAttributeBool(bone.isVisible           , pElement, "IsVisible");
        ParseAttributeInt(bone.rigidMatrixIndex     , pElement, "RigidMatrixIndex");
        ParseAttributeInt(bone.smoothMatrixIndex    , pElement, "SmoothMatrixIndex");
        ParseAttributeInt(bone.billboardIndex       , pElement, "BillboardIndex");
        ParseAttributeBool(bone.useRigidMatrix      , pElement, "UseRigidMatrix");
        ParseAttributeBool(bone.useSmoothMatrix     , pElement, "UseSmoothMatrix");
        ParseAttributeInt(bone.parentIndex          , pElement, "ParentIndex");
        ParseAttributeRotationType(bone.rotationType, pElement, "RotationType");
        ParseAttributeVector3F(bone.scale           , pElement, "Scale");
        ParseAttributeVector4F(bone.rotation        , pElement, "Rotation");
        ParseAttributeVector3F(bone.position        , pElement, "Position");
    }


	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
    void XmlParser::ParseTextureRefs(TextureRefs& textureRefs, Element* pElement)
    {
        pElement = pElement->first_node("TextureRefs");

        ParseAttributeBool(textureRefs.hasDiffuseMap  , pElement, "HasDiffuseMap");
        ParseAttributeBool(textureRefs.hasNormalMap   , pElement, "HasNormalMap");
        ParseAttributeBool(textureRefs.hasSpeculareMap, pElement, "HasSpecularMap");
        ParseAttributeBool(textureRefs.hasDiffuse2Map , pElement, "HasDiffuseLayer2Map");
        ParseAttributeBool(textureRefs.hasDiffuse3Map , pElement, "HasDiffuseLayer3Map");
        ParseAttributeBool(textureRefs.hasAOMap       , pElement, "HasAmbientOcclusionMap");
        ParseAttributeBool(textureRefs.hasEmissionMap , pElement, "HasEmissionMap");
        ParseAttributeBool(textureRefs.hasShadowMap   , pElement, "HasShadowMap");
        ParseAttributeBool(textureRefs.hasLightMap    , pElement, "HasLightMap");
        ParseAttributeBool(textureRefs.hasRoughnessMap, pElement, "HasRoughnessMap");
        ParseAttributeBool(textureRefs.hasMetalnessMap, pElement, "HasMetalnessMap");
        ParseAttributeBool(textureRefs.hasSSSMap      , pElement, "HasSubSurfaceScatteringMap");
        ParseAttributeUInt(textureRefs.textureCount   , pElement, "TextureCount");

        pElement = pElement->first_node("Texture");
		while (pElement)
		{
			TextureRef texture;

			ParseAttributeString             (texture.name               , pElement, "TextureName");
			ParseAttributeGX2TexClamp        (texture.clampX             , pElement, "ClampX");
			ParseAttributeGX2TexClamp        (texture.clampY             , pElement, "ClampY");
			ParseAttributeGX2TexClamp        (texture.clampZ             , pElement, "ClampZ");
			ParseAttributeString             (texture.samplerName        , pElement, "TexSamplerName");
			ParseAttributeString             (texture.useSampler         , pElement, "UseSampler");
			ParseAttributeGX2TexXYFilterType (texture.minFilter          , pElement, "MinFilter");
			ParseAttributeGX2TexXYFilterType (texture.magFilter          , pElement, "MagFilter");
			ParseAttributeGX2TexZFilterType  (texture.zFilter            , pElement, "ZFilter");
			ParseAttributeGX2TexMipFilterType(texture.mipFilter          , pElement, "MipFilter");
			ParseAttributeGX2TexBorderType   (texture.borderType         , pElement, "BorderType");
			ParseAttributeGX2CompareFunction (texture.depthCompareFunc   , pElement, "DepthCompareFunc");
			ParseAttributeFloat              (texture.minLod             , pElement, "MinLod");
			ParseAttributeFloat              (texture.maxLod             , pElement, "MaxLod");
			ParseAttributeFloat              (texture.lodBias            , pElement, "LodBias");
			ParseAttributeBool               (texture.depthCompareEnabled, pElement, "DepthCompareEnabled");
			ParseAttributeGX2TextureMapType  (texture.type               , pElement, "Type");
			ParseAttributeUInt               (texture.textureUnit        , pElement, "textureUnit");

			textureRefs.textures.push_back(texture);
			pElement = pElement->next_sibling("Texture");
		}
    }


	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
	void XmlParser::ParseMaterials(std::vector<FMAT>& fmats, Element* pElement)
	{
		// Parse FMATs
		Element* pNode = pElement->first_node("FMAT");
		while (pNode)
		{
            FMAT fmat;
            ParseFMAT(fmat, pNode);
			fmats.push_back(fmat);
			pNode = pNode->next_sibling("FMAT");
		}
	}


	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
	void XmlParser::ParseFMAT(FMAT& fmat, Element* pElement)
	{
		ParseAttributeString(fmat.name, pElement, "Name");
		ParseAttributeBool(fmat.isVisible, pElement, "IsVisible");
		// TODO write parsing functions for other FMAT members (shaderparams, etc)
		ParseTextureRefs(fmat.textureRefs, pElement);
	}

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseShapes(uint32 modelIndex, std::vector<FSHP>& fshps, Element* pElement)
    {
        // Parse FSHPs
        Element* pNode = pElement->first_node("FSHP");
        while (pNode)
        {
            FSHP fshp;
            fshp.modelIndex = modelIndex;
            ParseFSHP(fshp, pNode);
            fshps.push_back(fshp);
            pNode = pNode->next_sibling("FSHP");
        }
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFSHP(FSHP& fshp, Element* pElement)
    {
        ParseAttributeString(fshp.name, pElement, "Name");
        // TODO ParseAttributeShapeFlags
        ParseAttributeUInt(fshp.materialIndex, pElement, "MaterialIndex");
        ParseAttributeUInt(fshp.boneIndex, pElement, "BoneIndex");
        ParseAttributeUInt(fshp.vertexBufferIndex, pElement, "VertexBufferIndex");
        ParseAttributeFloatArray(fshp.radiusArray, pElement, "RadiusArray");
        ParseAttributeUInt(fshp.vertexSkinCount, pElement, "VertexSkinCount");
        ParseAttributeUInt(fshp.targetAttributeCount, pElement, "TargetAttributeCount");
        // TODO MORE CHILDREN

        // Parse Meshes
        Element* pNode = pElement->first_node("Meshes")->first_node("LODMesh");
        while (pNode)
        {
            LODMesh lodMesh;
            ParseLODMesh(lodMesh, pNode);
            fshp.lodMeshes.push_back(lodMesh);
            pNode = pNode->next_sibling("LODMesh");
        }

        // Parse Vertices
        pNode = pElement->first_node("Vertices")->first_node("Vertex");
        while (pNode)
        {
            FVTX fvtx;
            ParseFVTX(fvtx, pNode);
            fshp.vertices.push_back(fvtx);
            pNode = pNode->next_sibling("Vertex");
        }

    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseLODMesh(LODMesh& lodMesh, Element* pElement)
    {
        // Parse primitive and index type for real instead of hardcoding these values
        lodMesh.primitiveType = GX2PrimitiveType::Triangles;
        lodMesh.indexFormat = GX2IndexFormat::UInt16;
        ParseAttributeUInt(lodMesh.indexCount      , pElement, "IndexCount"  );
        ParseAttributeUInt(lodMesh.firstVertex     , pElement, "FirstVertex" );
        ParseAttributeIntArray(lodMesh.faceVertices, pElement, "FaceVertices");
        // TODO add parse submesh function
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    void XmlParser::ParseFVTX(FVTX& fvtx, Element* pElement)
    {
        ParseAttributeUInt(fvtx.index           , pElement, "Index"       );
        ParseAttributeVector3F(fvtx.position0   , pElement, "Position0"   );
        ParseAttributeVector3F(fvtx.position1   , pElement, "Position1"   );
        ParseAttributeVector3F(fvtx.position2   , pElement, "Position2"   );
        ParseAttributeVector3F(fvtx.normal      , pElement, "Normal"      );
        ParseAttributeVector2F(fvtx.uv0         , pElement, "UV0"         );
        ParseAttributeVector2F(fvtx.uv1         , pElement, "UV1"         );
        ParseAttributeVector2F(fvtx.uv2         , pElement, "UV2"         );
        ParseAttributeVector4F(fvtx.color0      , pElement, "Color0"      );
        ParseAttributeVector4F(fvtx.color1      , pElement, "Color1"      );
        ParseAttributeVector4F(fvtx.tangent     , pElement, "Tangent"     );
        ParseAttributeVector4F(fvtx.binormal    , pElement, "Binormal"    );
        ParseAttributeVector4F(fvtx.blendWeights, pElement, "BlendWeights");
        ParseAttributeVector4(fvtx.blendIndex   , pElement, "BlendIndex"  );
    }

}
