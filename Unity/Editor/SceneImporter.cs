using UnityEngine;
using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEditor.AssetImporters;
using UnityEngine.SceneManagement;
using System.IO;
using UnityEditor.SceneManagement;
using System.Xml.Schema;

[Serializable]
public class JsonTag
{
    public string Tag;
}

[Serializable]
public class JsonMeshRenderer
{
    public string PrimitiveType;
    public string SourceType;
}

[Serializable]
public class JsonTransform
{
    public float[] Rotation;
    public float[] Scale;
    public float[] Translation;

    public Vector3 GetTranslation() => (Translation != null && Translation.Length == 3) ? new Vector3(Translation[0], Translation[1], Translation[2]) : Vector3.zero;
    public Vector3 GetRotation() => (Rotation != null && Rotation.Length == 3) ? new Vector3(Rotation[0], Rotation[1], Rotation[2]) : Vector3.zero;
    public Vector3 GetScale() => (Scale != null && Scale.Length == 3) ? new Vector3(Scale[0], Scale[1], Scale[2]) : Vector3.zero;
}
[Serializable]
public class JsonBoxCollider
{
    public float Density;
    public float Friction;
    public float[] Offset;
    public float Restitution;
    public float[] Size;

    public Vector3 GetOffset() => (Offset != null && Offset.Length == 3) ? new Vector3(Offset[0], Offset[1], Offset[2]) : Vector3.zero;
    public Vector3 GetSize() => (Size != null && Size.Length == 3) ? new Vector3(Size[0] * 2.0f, Size[1] * 2.0f, Size[2] * 2.0f) : Vector3.zero;
}
[Serializable]
public class JsonSphereCollider
{
    public float Density;
    public float Friction;
    public float[] Offset;
    public float Radius;
    public float Restitution;


    public Vector3 GetOffset() => (Offset != null && Offset.Length == 3) ? new Vector3(Offset[0], Offset[1], Offset[2]) : Vector3.zero;
}
[Serializable]
public class JsonCapsuleCollider
{
    public float Density;
    public float Friction;
    public float Height;
    public float[] Offset;
    public float Radius;
    public float Restitution;



    public Vector3 GetOffset() => (Offset != null && Offset.Length == 3) ? new Vector3(Offset[0], Offset[1], Offset[2]) : Vector3.zero;
}
[Serializable]
public class JsonRigidbody
{
    public string Type;
}
[Serializable]
public class JsonEntity
{
    public JsonBoxCollider Box3DColliderComponent;
    public JsonCapsuleCollider CapsuleColliderComponent;
    public ulong Entities;
    public JsonMeshRenderer MeshRendererComponent;
    public JsonRigidbody RigidBody3DComponent;
    public JsonSphereCollider SphereColliderComponent;
    public JsonTag TagComponent;
    public JsonTransform TransformComponent;
}

[Serializable]
public class JsonScene
{
    public JsonEntity[] Entities;
    public string Scene;
}

[ScriptedImporter(1,"level")]
public class SceneImporter : ScriptedImporter
{
    public override void OnImportAsset(AssetImportContext context)
    {
        var file = File.ReadAllText(context.assetPath);
        var scene = JsonUtility.FromJson<JsonScene>(file);

        if (scene == null || scene.Entities == null)
        {
            context.LogImportError("Error in Importing");
            return;
        }
        for (int i = 0; i < scene.Entities.Length; i++)
        {
            var e = scene.Entities[i];
            
        }
        Scene importedScene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Additive);

        foreach(var entity in scene.Entities)
        {
            GameObject gameObject = null;
            string primitiveType = entity.MeshRendererComponent?.PrimitiveType;
            string sourceType = entity.MeshRendererComponent?.SourceType;

            if(primitiveType == "Cube")
            {
                gameObject = GameObject.CreatePrimitive(PrimitiveType.Cube);
            }
            else if(primitiveType == "Sphere")
            {
                gameObject = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            }
            else if(primitiveType == "Plane")
            {
                gameObject = GameObject.CreatePrimitive(PrimitiveType.Plane);
            }
            else if(primitiveType == "Capsule")
            {
                gameObject = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            }
            else
            {
                gameObject = new GameObject();
            }

            if(entity.TagComponent != null)
                gameObject.name = entity.TagComponent.Tag;

            if(entity.RigidBody3DComponent != null && !string.IsNullOrEmpty(entity.RigidBody3DComponent.Type))
            {
                Rigidbody rb = gameObject.AddComponent<Rigidbody>();
                rb.isKinematic = entity.RigidBody3DComponent.Type == "Static" ? true : false;
            }
            if(entity.Box3DColliderComponent != null)
            {
                var collider = gameObject.GetComponent<BoxCollider>();
                if (collider != null)
                {
                    collider.center = entity.Box3DColliderComponent.GetOffset();
                    collider.size = entity.Box3DColliderComponent.GetSize();

                    Rigidbody rb = collider.GetComponent<Rigidbody>();
                    if (rb != null)
                    {
                        rb.SetDensity(entity.Box3DColliderComponent.Density);
                    }

                    PhysicsMaterial pMaterial = new PhysicsMaterial();
                    pMaterial.frictionCombine = PhysicsMaterialCombine.Average;
                    pMaterial.bounceCombine = PhysicsMaterialCombine.Average;
                    pMaterial.dynamicFriction = entity.Box3DColliderComponent.Friction;
                    pMaterial.staticFriction = entity.Box3DColliderComponent.Friction;
                    pMaterial.bounciness = entity.Box3DColliderComponent.Restitution;

                    collider.material = pMaterial;
                }
            }

            if(entity.SphereColliderComponent != null)
            {
                var collider = gameObject.GetComponent<SphereCollider>();
                if(collider != null)
                { 
                    collider.center = entity.SphereColliderComponent.GetOffset();
                    collider.radius = entity.SphereColliderComponent.Radius;

                    Rigidbody rb = collider.GetComponent<Rigidbody>();
                    if(rb != null)
                    {
                        rb.SetDensity(entity.SphereColliderComponent.Density);
                    }

                    PhysicsMaterial pMaterial = new PhysicsMaterial();
                    pMaterial.frictionCombine = PhysicsMaterialCombine.Average;
                    pMaterial.bounceCombine = PhysicsMaterialCombine.Average;
                    pMaterial.dynamicFriction = entity.SphereColliderComponent.Friction;
                    pMaterial.staticFriction = entity.SphereColliderComponent.Friction;
                    pMaterial.bounciness = entity.SphereColliderComponent.Restitution;

                    collider.material = pMaterial;
                }
            }
            if(entity.CapsuleColliderComponent != null)
            {
                var collider = gameObject.GetComponent<CapsuleCollider>();
                if (collider != null)
                {
                    collider.center = entity.CapsuleColliderComponent.GetOffset();
                    collider.radius = entity.CapsuleColliderComponent.Radius;
                    collider.height = entity.CapsuleColliderComponent.Height;

                    Rigidbody rb = collider.GetComponent<Rigidbody>();
                    if (rb != null)
                    {
                        rb.SetDensity(entity.CapsuleColliderComponent.Density);
                    }

                    PhysicsMaterial pMaterial = new PhysicsMaterial();
                    pMaterial.frictionCombine = PhysicsMaterialCombine.Average;
                    pMaterial.bounceCombine = PhysicsMaterialCombine.Average;
                    pMaterial.dynamicFriction = entity.CapsuleColliderComponent.Friction;
                    pMaterial.staticFriction = entity.CapsuleColliderComponent.Friction;
                    pMaterial.bounciness = entity.CapsuleColliderComponent.Restitution;

                    collider.material = pMaterial;
                }
            }

            SceneManager.MoveGameObjectToScene(gameObject, importedScene);

            if(entity.TransformComponent != null)
            {
                gameObject.transform.position = entity.TransformComponent.GetTranslation();
                gameObject.transform.rotation = Quaternion.Euler(entity.TransformComponent.GetRotation());
                gameObject.transform.localScale = entity.TransformComponent.GetScale();
            }

        }

        EditorSceneManager.SaveScene(importedScene);
        EditorSceneManager.CloseScene(importedScene, true);
    }

}

public static class SceneExporter
{
    [MenuItem("Tools/Export Scene To Json")]
    public static void ExportSceneToJson()
    {
        Scene unityScene = SceneManager.GetActiveScene();

        JsonScene jsonScene = new JsonScene();
        jsonScene.Scene = unityScene.name;

        GameObject[] allGameobjects = unityScene.GetRootGameObjects();
        List<JsonEntity> jsonEntities = new List<JsonEntity>();

        foreach(GameObject entity in allGameobjects)
        {
            JsonEntity jsonEntity = new JsonEntity();
            jsonEntity.Entities = (ulong)entity.GetInstanceID();

            jsonEntity.TagComponent = new JsonTag{ Tag = entity.name };
            jsonEntity.TransformComponent = new JsonTransform
            {
                Translation = new float[] { entity.transform.position.x, entity.transform.position.y, entity.transform.position.z },
                Rotation = new float[] { entity.transform.eulerAngles.x, entity.transform.eulerAngles.y, entity.transform.eulerAngles.z },
                Scale = new float[] { entity.transform.localScale.x, entity.transform.localScale.y, entity.transform.localScale.z }
            };

            MeshFilter filter = entity.GetComponent<MeshFilter>();
            if(filter != null && filter.sharedMesh != null)
            {
                jsonEntity.MeshRendererComponent = new JsonMeshRenderer();
                string meshName = filter.sharedMesh.name;

                if (meshName.Contains("Cube")) { jsonEntity.MeshRendererComponent.PrimitiveType = "Cube"; jsonEntity.MeshRendererComponent.SourceType = "Primitive"; }
                else if (meshName.Contains("Sphere")) { jsonEntity.MeshRendererComponent.PrimitiveType = "Sphere"; jsonEntity.MeshRendererComponent.SourceType = "Primitive"; }
                else if (meshName.Contains("Plane")) { jsonEntity.MeshRendererComponent.PrimitiveType = "Plane"; jsonEntity.MeshRendererComponent.SourceType = "Primitive"; }
                else if (meshName.Contains("Capsule")) { jsonEntity.MeshRendererComponent.PrimitiveType = "Capsule"; jsonEntity.MeshRendererComponent.SourceType = "Primitive"; }
                else { jsonEntity.MeshRendererComponent.PrimitiveType = "None"; jsonEntity.MeshRendererComponent.SourceType = "Asset"; }
                ;
            }

            BoxCollider bc = entity.GetComponent<BoxCollider>();
            
            if(bc != null)
            {

                Rigidbody rbbc = bc.GetComponent<Rigidbody>();
                float calculatedDensity = 1.0f;
                if(rbbc != null)
                {
                    Vector3 size = bc.size;
                    float volume = size.x * size.y * size.z * entity.transform.lossyScale.x * entity.transform.lossyScale.y * entity.transform.lossyScale.z;

                    calculatedDensity = rbbc.mass / volume;
                }
                

                jsonEntity.Box3DColliderComponent = new JsonBoxCollider
                {
                    Density = calculatedDensity,
                    Offset = new float[] { bc.center.x, bc.center.y, bc.center.z },
                    Size = new float[] { bc.size.x * 0.5f, bc.size.y * 0.5f, bc.size.z * 0.5f },
                    Friction = bc.sharedMaterial != null ? bc.sharedMaterial.staticFriction : 0.6f,
                    Restitution = bc.sharedMaterial != null ? bc.sharedMaterial.bounciness : 0.0f
                };

            }
            MeshCollider mc = entity.GetComponent<MeshCollider>();
            if (mc != null)
            {
                if (entity.GetComponent<BoxCollider>() == null)
                {
                    entity.AddComponent<BoxCollider>();
                }
                BoxCollider mcol = mc.GetComponent<BoxCollider>();
                Rigidbody rbbc = mc.GetComponent<Rigidbody>();
                float calculatedDensity = 1.0f;
                if (rbbc == null)
                {
                    entity.AddComponent<Rigidbody>();
                    var rbmb = entity.GetComponent<Rigidbody>();
                    rbmb.isKinematic = true;
                    Vector3 size = mcol.size;
                    float volume = size.x * size.y * size.z * entity.transform.lossyScale.x * entity.transform.lossyScale.y * entity.transform.lossyScale.z;

                    calculatedDensity = rbbc.mass / volume;
                }
       
                jsonEntity.Box3DColliderComponent = new JsonBoxCollider
                {
                    Density = calculatedDensity,
                    Offset = new float[] { mcol.center.x, mcol.center.y, mcol.center.z },
                    Size = new float[] { mcol.size.x * 0.5f, 0.001f, mcol.size.z * 0.5f },
                    Friction = mcol.sharedMaterial != null ? mcol.sharedMaterial.staticFriction : 0.6f,
                    Restitution = mcol.sharedMaterial != null ? mcol.sharedMaterial.bounciness : 0.0f
                };
                
            }
            SphereCollider sc = entity.GetComponent<SphereCollider>();
            if(sc != null)
            {

                Rigidbody rbsc = sc.GetComponent<Rigidbody>();
                float calculatedDensity = 1.0f;
                if(rbsc != null)
                {
                    Vector3 lossyScale = entity.transform.lossyScale;
                    float maxScale = Mathf.Max(lossyScale.x, Math.Max(lossyScale.y, lossyScale.z));
                    float scaledRadius = sc.radius * maxScale;

                    float volume = (4f/3f) * Mathf.PI * scaledRadius * scaledRadius * scaledRadius;

                    calculatedDensity = rbsc.mass / volume;
                }
                jsonEntity.SphereColliderComponent = new JsonSphereCollider
                {
                    Density = calculatedDensity,
                    Offset = new float[] { sc.center.x, sc.center.y, sc.center.z },
                    Radius = sc.radius,
                    Friction = sc.sharedMaterial != null ? sc.sharedMaterial.staticFriction : 0.6f,
                    Restitution = sc.sharedMaterial != null ? sc.sharedMaterial.bounciness : 0.0f
                };
            }
            CapsuleCollider cc = entity.GetComponent<CapsuleCollider>();
            if(cc != null)
            {

                Rigidbody rbcc = cc.GetComponent<Rigidbody>();
                float calculatedDensity = 1.0f;
                if(rbcc != null)
                {
                    Vector3 lossyScale = entity.transform.lossyScale;

                    float heightAxisScale = 0.0f;
                    float radiusAxisScaleA = 0.0f;
                    float radiusAxisScaleB = 0.0f;

                    switch(cc.direction)
                    {
                        case 0:
                            heightAxisScale = lossyScale.x;
                            radiusAxisScaleA = lossyScale.y;
                            radiusAxisScaleB = lossyScale.z;
                            break;
                        case 2:
                            heightAxisScale = lossyScale.z;
                            radiusAxisScaleA = lossyScale.x;
                            radiusAxisScaleB = lossyScale.y;
                            break;
                        default:
                            heightAxisScale = lossyScale.y;
                            radiusAxisScaleA = lossyScale.x;
                            radiusAxisScaleB = lossyScale.z;
                            break;
                    }

                    float scaledRadius = cc.radius * Mathf.Max(radiusAxisScaleA, radiusAxisScaleB);
                    float scaledHeight = cc.height * heightAxisScale;
                    scaledHeight = Mathf.Max(scaledHeight, scaledRadius * 2f);
                    scaledHeight = Mathf.Max(scaledHeight, scaledRadius * 2f);

                    float cylinderHeight = scaledHeight - (2f * scaledRadius);
                    float volume = (Mathf.PI * scaledRadius * scaledRadius * cylinderHeight) + (4f / 3f) * Mathf.PI * scaledRadius * scaledRadius * scaledRadius;

                    calculatedDensity = rbcc.mass / volume;
                }
                jsonEntity.CapsuleColliderComponent = new JsonCapsuleCollider
                {
                    Density = calculatedDensity,
                    Offset = new float[] { cc.center.x, cc.center.y, cc.center.z },
                    Radius = cc.radius,
                    Height = cc.height,
                    Friction = cc.sharedMaterial != null ? cc.sharedMaterial.staticFriction : 0.6f,
                    Restitution = cc.sharedMaterial != null ? cc.sharedMaterial.bounciness : 0.0f
                };
            }

            Rigidbody rb = entity.GetComponent<Rigidbody>();
            if (rb != null)
            {
                jsonEntity.RigidBody3DComponent = new JsonRigidbody
                {
                    Type = rb.isKinematic ? "Static" : "Dynamic"
                };

            }

            jsonEntities.Add(jsonEntity);
        }

        jsonScene.Entities = jsonEntities.ToArray();
        string defaultFileName = $"{unityScene.name}";
        string savePath = EditorUtility.SaveFilePanel("Save level", Application.dataPath, defaultFileName, "level");

        string ToJson = SceneJsonWriter.WriteScene(jsonScene);
        File.WriteAllText(savePath, ToJson);
        AssetDatabase.Refresh();
    }
}


public static class SceneJsonWriter
{
    public static string WriteScene(JsonScene scene)
    {
        var stringBuilder = new System.Text.StringBuilder();
        stringBuilder.Append("{\n");
        stringBuilder.Append("  \"Entities\" :\n");
        stringBuilder.Append("  [\n");

        for(int i = 0; i <scene.Entities.Length; i++)
        {
            stringBuilder.Append(IndentLines(WriteEntity(scene.Entities[i]), "    "));
            if (i < scene.Entities.Length - 1)
                stringBuilder.Append(",");
            stringBuilder.Append("\n");
        }
        stringBuilder.Append("  ],\n");
        stringBuilder.Append($"  \"Scene\" : {EscapeString(scene.Scene)}\n");
        stringBuilder.Append("}");
        return stringBuilder.ToString();

    }
    
    private static string WriteEntity(JsonEntity entity)
    {
        var fields = new SortedDictionary<string, string>(StringComparer.Ordinal);

        fields["Entity"] = entity.Entities.ToString();

        if(entity.TagComponent != null)
            fields["TagComponent"] = JsonUtility.ToJson(entity.TagComponent, true);
        if(entity.TransformComponent != null)
            fields["TransformComponent"] = JsonUtility.ToJson(entity.TransformComponent, true);
        if(entity.Box3DColliderComponent != null)
            fields["Box3DColliderComponent"] = JsonUtility.ToJson(entity.Box3DColliderComponent, true);
        if(entity.CapsuleColliderComponent != null)
            fields["CapsuleColliderComponent"] = JsonUtility.ToJson(entity.CapsuleColliderComponent, true);
        if(entity.MeshRendererComponent != null)
            fields["MeshRendererComponent"] = JsonUtility.ToJson(entity.MeshRendererComponent, true);
        if (entity.RigidBody3DComponent != null)
            fields["RigidBody3DComponent"] = JsonUtility.ToJson(entity.RigidBody3DComponent, true);
        if(entity.SphereColliderComponent  != null)
            fields["SphereColliderComponent"] = JsonUtility.ToJson(entity.SphereColliderComponent, true);
        var lines = new List<string>();

        foreach(var keyValue in fields)
        {
            string value = keyValue.Value.Contains("\n") ? IndentLines(keyValue.Value, "  ").TrimStart() : keyValue.Value;
            lines.Add($"\"{keyValue.Key}\" : {value}");
        }

        return "{\n  " + string.Join(",\n  ", lines) + "\n}";
    }

    private static string EscapeString(string str)
    {
        if (str == null) return "null";
        return "\"" + str.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\"";
    }
    private static string IndentLines(string text, string indent)
    {
        var lines = text.Split('\n');
        for(int i = 0; i < lines.Length; i++)
        {
            lines[i] = indent + lines[i];
        }
        return string.Join("\n", lines);
    }
}