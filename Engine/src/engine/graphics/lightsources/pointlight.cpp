//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "pointlight.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "shader.hpp"
#include "render.hpp"
#include "texture.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using std::make_shared;

using Core::Engine;
using Core::ECS::GameObject;
using Core::ECS::Transform;
using Core::ECS::Mesh;
using Core::ECS::Material;
using Graphics::Shader;
using Graphics::Render;
using Graphics::Texture;

namespace Graphics::LightSources
{
	GameObject PointLight::CreatePointLight(const vec3& position, const vec3& scale, const vec3& color, float shininess)
	{
		GameObject obj;
		obj.SetName("Point light");
		obj.SetType(GameObject::Type::point_light);
		obj.SetID();

		Transform transform(position, vec3(0.0f, 0.0f, 0.0f), scale);
		transform.SetPosition(position);
		transform.SetRotation(vec3(0.0f, 0.0f, 0.0f));
		transform.SetScale(scale);
		obj.AddComponent(make_shared<Transform>(transform));

		float vertices[] =
		{
			//positions          //normals
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		Mesh mesh(vertices);
		mesh.SetVertices(vertices);
		obj.AddComponent(make_shared<Mesh>(mesh));

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);

		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		Shader TestLightShader = Shader(
			Engine::enginePath + "/shaders/Light_Test.vert",
			Engine::enginePath + "/shaders/Light_Test.frag");

		Material mat(color, shininess, VAO, VBO, TestLightShader);
		mat.SetVAO(VAO);
		mat.SetVBO(VBO);
		mat.SetShader(TestLightShader);
		obj.AddComponent(make_shared<Material>(mat));

		obj.Initialize();

		Render::gameObjects.push_back(obj);
		Render::pointLights.push_back(obj);

		return obj;
	}

	void PointLight::RenderPointLight(GameObject& obj, mat4& view, mat4& projection)
	{
		Shader shader = obj.GetComponent<Material>()->GetShader();

		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);
		shader.SetVec3("lightColor", Render::pointDiffuse);

		mat4 model = mat4(1.0f);
		model = translate(model, obj.GetComponent<Transform>()->GetPosition());
		quat newRot = quat(radians(obj.GetComponent<Transform>()->GetRotation()));
		model *= mat4_cast(newRot);
		model = scale(model, obj.GetComponent<Transform>()->GetScale());

		shader.SetMat4("model", model);
		GLuint VAO = obj.GetComponent<Material>()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}