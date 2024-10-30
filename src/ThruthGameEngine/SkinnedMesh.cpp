#include "SkinnedMesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "IAnimation.h"
#include "Entity.h"
#include "IBone.h"
#include "Imesh.h"
#include "IMaterial.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SkinnedMesh)

/// <summary>
/// 경로를 지정한 생성자
/// </summary>
/// <param name="_path">매쉬 정보가 있는 경로</param>
Truth::SkinnedMesh::SkinnedMesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_animationMaxFrame(0)
	, m_oldFrame(0)
	, m_isRootMotion(true)
	, m_prevMovement(0.0f)
	, m_currentMovement(0.0f)
{
	m_name = "Skinned Mesh Filter";
}

/// <summary>
/// 경로가 지정되지 않은 생성자
/// 기본 카츄진 매쉬로 생성해준다.
/// </summary>
Truth::SkinnedMesh::SkinnedMesh()
	: Component()
	, m_path(L"Kachujin/Mesh")
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_animationMaxFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_oldFrame(0)
	, m_isRootMotion(true)
	, m_prevMovement(0.0f)
	, m_currentMovement(0.0f)
{
	m_name = "Skinned Mesh Filter";
}

/// <summary>
/// 소멸자
/// </summary>
Truth::SkinnedMesh::~SkinnedMesh()
{
	DeleteMesh();
}

/// <summary>
/// 매쉬 세팅 함수
/// 경로에 있는 매쉬 데이터로 매쉬를 설정해준다.
/// </summary>
/// <param name="_path">매쉬 데이터 경로</param>
void Truth::SkinnedMesh::SetSkinnedMesh(std::wstring _path)
{
	m_path = _path;
	fs::path meshPath = m_path;

	// 이미 매쉬가 있다면 지우고 새로 만든다.
	if (m_skinnedMesh != nullptr)
	{
		DeleteMesh();
		m_skinnedMesh.reset();
	}
	// 매쉬 생성
	m_skinnedMesh = m_managers.lock()->Graphics()->CreateSkinnedMesh(_path);

	// 본 정보 리로드
	m_boneMap.clear();
	uint32 boneSize = m_skinnedMesh->GetBonesSize();
	for (uint32 i = 0; i < boneSize; i++)
	{
		std::weak_ptr<Ideal::IBone> bone = m_skinnedMesh->GetBoneByIndex(i);
		m_boneMap[bone.lock()->GetName()] = bone;
	}

	// 머테리얼 정보가 저장되어있지 않다면
	// 기본 머테리얼을 정보를 생성하여 저장한다.
	if (m_matPath.empty())
	{
		for (size_t i = 0; i < m_skinnedMesh->GetMeshesSize(); i++)
		{
			std::string matName = m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->GetFBXMaterialName();
			fs::path matPath = "../Resources/Matarial" / meshPath.filename();

			if (!fs::exists(matPath))
				fs::create_directories(matPath);

			matPath = matPath.filename() / matName;

			auto material = m_managers.lock()->Graphics()->CreateMaterial(matPath.generic_string());
			m_mat.push_back(material);

			m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);
		}
	}
	// 저장되어 있는 머테리얼 정보를 불러온다.
	else
	{
		for (size_t i = 0; i < m_skinnedMesh->GetMeshesSize(); i++)
		{
			auto material = m_managers.lock()->Graphics()->CreateMaterial(m_matPath[i], false);
			m_mat.push_back(material);

			m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);
		}
	}

	m_rootBone = m_skinnedMesh->GetBoneByIndex(1);
	SetRootMotion(true);
}

/// <summary>
/// 매쉬에 애니메이션 추가
/// </summary>
/// <param name="_name">애니메이션 이름</param>
/// <param name="_path">애니메이션 데이터 경로</param>
/// <param name="_offset">애니메이션 오프셋 매트릭스</param>
void Truth::SkinnedMesh::AddAnimation(std::string _name, std::wstring _path, const Matrix& _offset /*= Matrix::Identity*/)
{
	if (m_animation != nullptr)
		m_animation.reset();

	m_animation = m_managers.lock()->Graphics()->CreateAnimation(_path, _offset);

	if (m_skinnedMesh != nullptr)
		m_skinnedMesh->AddAnimation(_name, m_animation);
}

/// <summary>
/// 현재 재생할 애니메이션 설정
/// </summary>
/// <param name="_name">애니메이션 이름</param>
/// <param name="WhenCurrentAnimationFinished">현재 애니메이션이 종료 되고 재생할지 여부</param>
void Truth::SkinnedMesh::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	if (m_animation != nullptr)
	{
		m_isAnimationChanged = true;
		m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
	}
}

/// <summary>
/// 애니메이션 재생 속도 지정
/// </summary>
/// <param name="Speed">속도 (배율)</param>
void Truth::SkinnedMesh::SetAnimationSpeed(float Speed)
{
	m_skinnedMesh->SetAnimationSpeed(Speed);
}

/// <summary>
/// 애니메이션 재생 여부 지정
/// </summary>
/// <param name="playStop">재생 여부</param>
void Truth::SkinnedMesh::SetPlayStop(bool playStop)
{
	m_skinnedMesh->SetPlayAnimation(playStop);
}

void Truth::SkinnedMesh::Initialize()
{
	SetSkinnedMesh(m_path);
	if (m_skinnedMesh)
		m_skinnedMesh->SetDrawObject(m_owner.lock()->m_isActive);
}

void Truth::SkinnedMesh::Update()
{
	// 애니메이션이 변경 되지 않았다면 애니메이션 현재 프레임을 받아온다.
	if (!m_isAnimationChanged)
		m_currentFrame = m_skinnedMesh->GetCurrentAnimationIndex();
	// 만일 변경 되었다면
	else
	{
		// 프레임 정보를 0으로 초기화 한다.
		if (m_skinnedMesh->GetCurrentAnimationIndex() == 0)
		{
			m_currentFrame = 0;
			m_oldFrame = 0;
			m_isAnimationChanged = false;
		}
	}

	// 애니메이션이 있다면
	// 해당 에니메이션을 재생한다.
	if (m_animation != nullptr)
	{
		if (m_owner.lock()->m_name == "Boss")
		{
			int a = 1;
		}
		m_animationMaxFrame = m_skinnedMesh->GetCurrentAnimationMaxFrame();
		m_skinnedMesh->AnimationDeltaTime(GetDeltaTime());

		if (m_oldFrame > m_currentFrame)
		{
			if (m_owner.lock()->m_name == "Boss")
			{
				int a = 1;
			}
			m_isAnimationEnd = true;
			m_isAnimationPlaying = false;
			m_prevMovement = Vector3::Zero;
			m_currentMovement = Vector3::Zero;
		}
		else
		{
			m_isAnimationEnd = false;
			m_isAnimationPlaying = true;
			if (m_isRootMotion)
			{
				Vector3 tempSca;
				Quaternion tempRot;
				Matrix btm = m_rootBone.lock()->GetTransform();
				btm.Decompose(tempSca, tempRot, m_currentMovement);
				Translate(m_currentMovement - m_prevMovement);
				m_prevMovement = m_currentMovement;
			}
		}
		if (m_owner.lock()->m_name == "Boss")
		{
			// DEBUG_PRINT((std::to_string(m_oldFrame) + " " + std::to_string(m_currentFrame) + "\n").c_str());
		}

		m_oldFrame = m_currentFrame;
	}
}

/// <summary>
/// 매쉬의 위치 정보 반영
/// </summary>
void Truth::SkinnedMesh::ApplyTransform()
{

	m_skinnedMesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_skinnedMesh->SetDrawObject(m_isRendering);
}

/// <summary>
/// 매쉬 삭제
/// </summary>
void Truth::SkinnedMesh::Destroy()
{
	DeleteMesh();
}

/// <summary>
/// Active 여부
/// </summary>
void Truth::SkinnedMesh::SetActive()
{
	if (m_skinnedMesh)
		m_skinnedMesh->SetDrawObject(m_owner.lock()->m_isActive);
}

/// <summary>
/// 매쉬 삭제
/// </summary>
void Truth::SkinnedMesh::DeleteMesh()
{
	if (m_skinnedMesh)
		m_managers.lock()->Graphics()->DeleteMeshObject(m_skinnedMesh);
	m_skinnedMesh.reset();
	m_skinnedMesh = nullptr;
}

void Truth::SkinnedMesh::SetRootMotion(bool _isRootMotion)
{
	m_isRootMotion = _isRootMotion;
	// m_owner.lock()->m_transform->m_isRootMotion = m_isRootMotion;

	m_prevMovement = Vector3::Zero;
	m_currentMovement = Vector3::Zero;
}

/// <summary>
/// 이름으로 본을 검색해서 리턴한다.
/// </summary>
/// <param name="_name">이름</param>
/// <returns>본</returns>
std::weak_ptr<Ideal::IBone> Truth::SkinnedMesh::GetBone(const std::string& _name)
{
	if (m_boneMap.find(_name) == m_boneMap.end())
		return std::weak_ptr<Ideal::IBone>();
	return m_boneMap[_name];
}

#ifdef EDITOR_MODE
void Truth::SkinnedMesh::EditorSetValue()
{
	m_matPath.clear();
	SetSkinnedMesh(m_path);
}
#endif // EDITOR_MODE

