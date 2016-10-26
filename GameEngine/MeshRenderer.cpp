#include <sstream>
#include "MeshRenderer.h"
#include "Camera.h"
#include "Lighting/Light.h"
#include "Lighting/DirectionalLight.h"

namespace GameEngine
{
	MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
		material(material),
		mesh(mesh)
	{
	}

	void MeshRenderer::update(const UpdateInfo& updateInfo)
	{
		material->use();

		auto model = getGameObject()->getModelMatrix();
		auto view = getGameObject()->getCamera()->getViewMatrix();
		auto proj = getGameObject()->getCamera()->getProjMatrix();

		material->setUniform("modelMatrix", model);
		material->setUniform("viewMatrix", view);
		material->setUniform("projMatrix", proj);
		material->setUniform("mvpMatrix", proj * view * model);

		auto lights = LightManager::getInstance().getAllLights();
        for (int i = 0; i < lights.size(); ++i)
        {
            auto light = lights[i];
            if (dynamic_cast<DirectionalLight*>(light))
            {
                std::stringstream ss;
                ss << "dirLights[" << i << "]";
                material->setUniform(ss.str(), sizeof(DirectionalLight), reinterpret_cast<GLfloat*>(light));
            }
        }

		mesh->bind();
		glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_SHORT, 0);
	}

	void MeshRenderer::onDestroy()
	{
		material.reset();
		mesh.reset();
	}

	int MeshRenderer::getOrder() const
	{
		// Rendering happens late in the update loop
		return 1000;
	}
}