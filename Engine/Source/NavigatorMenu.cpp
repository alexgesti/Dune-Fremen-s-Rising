#include "Application.h"
#include "Globals.h"

#include "NavigatorMenu.h"
#include "InputGeom.h"
#include "NavMeshBuilder.h"

#include "IconsFontAwesome5.h"

#include "Glew/include/GL/glew.h"

NavigatorMenu::NavigatorMenu() : Menu(true)
{
}

NavigatorMenu::~NavigatorMenu()
{
}

bool NavigatorMenu::Start()
{
    buildSettings = &app->navMesh->GetBuildSettings();

    buildSettings->cellSize             = 0.3f;
    buildSettings->cellHeight           = 0.2f;
    buildSettings->agentHeight          = 2.0f;
    buildSettings->agentRadius          = 0.6f;
    buildSettings->agentMaxClimb        = 0.9f;
    buildSettings->agentMaxSlope        = 45.0f;
    buildSettings->regionMinSize        = 8;
    buildSettings->regionMergeSize      = 20;
    buildSettings->edgeMaxLen           = 12.0f;
    buildSettings->edgeMaxError         = 1.3f;
    buildSettings->vertsPerPoly         = 6.0f;
    buildSettings->detailSampleDist     = 6.0f;
    buildSettings->detailSampleMaxError = 1.0f;
    buildSettings->partitionType        = 0.0f;
    buildSettings->tileSize             = 32.0f;

    DEBUG_LOG("Linking the ModuleNavMesh Settings");
    bool ret = true;

    return ret;
}

bool NavigatorMenu::Update(float dt)
{
    ImGui::Begin(ICON_FA_WALKING" Navigator", &active);
    if (ImGui::Button("Bake"))
    {
        app->navMesh->BakeNavMesh();
    }
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Agent Properties");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::DragFloat("Agent radius", &buildSettings->agentRadius, 0.1f);
    ImGui::DragFloat("Agent height", &buildSettings->agentHeight, 0.1f);
    ImGui::DragFloat("Agent max climb", &buildSettings->agentMaxClimb, 0.1f);
    ImGui::DragFloat("Agent max slope", &buildSettings->agentMaxSlope, 0.1f);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("NavMesh Properties");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::DragFloat("Cell size", &buildSettings->cellSize, 0.1f);
    ImGui::DragFloat("Cell hight", &buildSettings->cellHeight, 0.1f);
    ImGui::DragFloat("Region minimum size", &buildSettings->regionMinSize, 0.1f);
    ImGui::DragFloat("Region merge size", &buildSettings->regionMergeSize, 0.1f);
    ImGui::DragFloat("Edge max lenght", &buildSettings->edgeMaxLen, 0.1f);
    ImGui::DragFloat("Edge max error", &buildSettings->edgeMaxError, 0.1f);
    ImGui::DragFloat("Detail sample distance", &buildSettings->detailSampleDist, 0.1f);
    ImGui::DragFloat("Detail sample max error", &buildSettings->detailSampleMaxError, 0.1f);
    ImGui::DragFloat("Tile Size", &buildSettings->tileSize, 0.1f);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("NavMeshBuilder"))
    {
        ImGui::Text("NaveMesh Geometry");
        ImGui::Separator();
        ImGui::Spacing();
        
        const SimpleMesh* mesh = app->navMesh->GetInputGeom()->getMesh();
        const NavMeshBuilder* builder = app->navMesh->GetNavMeshBuilder();
        if (mesh->vertices.size() <= 0 && mesh->indices.size() <= 0)
            ImGui::Text("No Geometry added");
        else 
        {
            ImGui::Text("Verts: %d", mesh->vertices.size());
            ImGui::Text("Indices: %d", mesh->indices.size());

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("Max Tiles: %i", builder->GetMaxTyles());
            ImGui::Text("Max Polys per tiles: %i", builder->GetMaxPolyTile());
            ImGui::Text("Cell size: %.4f", buildSettings->cellSize);
            ImGui::Text("Cell Height: %.4f", buildSettings->cellHeight);

            if (builder->GetPolyMesh() != nullptr)
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("PolyMesh");

                ImGui::Text("Verts: %i", builder->GetPolyMesh()->nverts);
                ImGui::Text("Polys: %i", builder->GetPolyMesh()->npolys);
            }
        }
    }

    ImGui::End();
    return true;
}