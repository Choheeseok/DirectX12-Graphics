#include "pch.h"
#include "d3dUtil.h"
#include "Scene.h"

void Scene::CreateShaderVariables()
{
}

void Scene::UpdateShaderVariables()
{
	for (const auto& camera : m_umCameras)
		camera.second->UpdateShaderVariables(m_pd3dCommandList);

	for (const auto& shader : m_umShaders)
		shader.second->UpdateShaderVariables();
}

void Scene::SetShaderVariables()
{
}