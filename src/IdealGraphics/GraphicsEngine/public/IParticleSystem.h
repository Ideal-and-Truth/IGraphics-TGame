#pragma once
#include <memory>
#include "../Utils/SimpleMath.h"

namespace Ideal
{
	class IMesh;
	class IParticleMaterial;
}
namespace Ideal
{
	namespace ParticleMenu
	{
		// ���� ������ ���� Enum
		enum class ERangeMode
		{
			Constant = 0, // ��� 
			Curve,
			RandomBetweenConstants,	// �� ���� ��� ������ ���� ��
			RandomBetweenCurves,	// �� ���� Ŀ�� ������ ���� ��
		};

		enum class ERendererMode
		{
			Billboard,	// ī�޶� �ٶ�
			Mesh,	// �Ž� ���·� �Ž��� ���׸����� ��������.
		};

		struct ShaderData
		{
			DirectX::SimpleMath::Vector4 CustomData1;
			DirectX::SimpleMath::Vector4 CustomData2;
			float Time;
		};

		enum class ECustomData
		{
			CustomData_1 = 0,
			CustomData_2,
		};
		enum class ECustomDataParameter : unsigned int
		{
			CustomData_x = 0,
			CustomData_y,
			CustomData_z,
			CustomData_w
		};
	}
}
namespace Ideal
{
	class IParticleSystem
	{
	public:
		IParticleSystem() {};
		virtual ~IParticleSystem() {};

		// ���� �����ڸ� ������ ������
		// ON, OFF ��ɵ�
	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Transform) abstract;
		virtual const DirectX::SimpleMath::Matrix& GetTransformMatrix() const abstract;

	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) abstract;

		//---------Renderer---------//
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) abstract;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) abstract;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) abstract;
	};
}