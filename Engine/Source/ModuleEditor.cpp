﻿#include "ModuleEditor.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "GameObject.h"

#include "ConsoleMenu.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_sdl.h"

#include "Profiling.h"

ModuleEditor::ModuleEditor() : selected(nullptr), selectedParent(nullptr), currentOperation(ImGuizmo::OPERATION::TRANSLATE), Module()
{
	name = "Editor";

	viewport = new Viewport();
	gameView = new GameView();
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Start()
{
	mainMenuBar.Start();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	RG_PROFILING_FUNCTION("Updating Module Editor");
	bool ret = true;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_IDLE && selected)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_UP) currentOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_UP) currentOperation = ImGuizmo::OPERATION::ROTATE;
		else if (app->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_UP) currentOperation = ImGuizmo::OPERATION::SCALE;
	}


	ImGui::DockSpaceOverViewport();
	
	ret = mainMenuBar.Update(dt);

	if (app->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN)
	{
		app->window->SetFullscreen();
	}

	return ret;
}

bool ModuleEditor::Draw(Framebuffer* editorBuffer, Framebuffer* gameBuffer)
{
	RG_PROFILING_FUNCTION("Drawing Module Editor");
	
	viewport->Draw(editorBuffer, gameBuffer, currentOperation);
	gameView->Draw(gameBuffer);
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return true;
}

bool ModuleEditor::CleanUp()
{
	mainMenuBar.CleanUp();

	RELEASE(viewport);

	return true;
}

bool ModuleEditor::LoadConfig(JsonParsing& node)
{
	return true;
}

bool ModuleEditor::SaveConfig(JsonParsing& node) const
{
	return true;
}

void ModuleEditor::LogConsole(const char* string)
{
	if (mainMenuBar.GetConsole()) mainMenuBar.GetConsole()->AddLog(string);
}