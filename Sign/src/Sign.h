#pragma once

#include "signpch.h"

//For the App
#include "Sign/Application.h"
#include "Sign/Layer.h"
#include "Sign/Time.h"
#include "Sign/Timestep.h"
#include "Sign/Input.h"
#include "Sign/KeyCodes.h"
#include "Sign/MouseCodes.h"


#include "Sign/ImGui/ImGuiLayer.h"
#include "Sign/Renderer/CubeEntity.h"
#include "Sign/Renderer/SphereEntity.h"
#include "Sign/Renderer/PlaneEntity.h"
#include "Sign/Renderer/CircleEntity.h"
#include "Sign/Renderer/TriangleEntity.h"
#include "Sign/Renderer/EmptyEntity.h"

#include "Sign/Scene/Scene.h"
#include "Sign/Scene/EntityECS.h"
#include "Sign/Scene/ECS/Components.h"
#include "Sign/Renderer/Texture.h"

#include "Sign/Ccommand/EditorCommand.h"
#include "Sign/Buffers/FrameBuffer.h"

#include "Sign/Ccommand/CreateObjectCommand.h"
#include "Sign/Ccommand/RemoveObjectCommand.h"
#include "Sign/Ccommand/DeleteAllCommand.h"
