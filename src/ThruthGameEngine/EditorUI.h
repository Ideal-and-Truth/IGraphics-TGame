#pragma once
#include <memory>
#include <vector>
#include <string>
#include <windows.h>
#include "Types.h"
#include <queue>

#include "imgui.h"
#include "imgui_internal.h"
#include "GraphEditor.h"

namespace Truth
{
	class Entity;
	class Transform;
	class Managers;
	class Component;
	class AnimatorController;
	class AnimationInfo;
}

template <typename T, std::size_t N>
struct Array
{
	T data[N];
	const size_t size() const { return N; }

	const T operator [] (size_t index) const { return data[index]; }
	operator T* () {
		T* p = new T[N];
		memcpy(p, data, sizeof(data));
		return p;
	}
};

template <typename T, typename ... U> Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

struct GraphEditorDelegate : public GraphEditor::Delegate
{
	bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
	{
		return true;
	}

	void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
	{
		mNodes[nodeIndex].mSelected = selected;
	}

	void MoveSelectedNodes(const ImVec2 delta) override
	{
		for (auto& node : mNodes)
		{
			if (!node.mSelected)
			{
				continue;
			}
			node.x += delta.x;
			node.y += delta.y;
		}
	}

	virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
	{
	}

	void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
	{
		mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
	}

	void DelLink(GraphEditor::LinkIndex linkIndex) override
	{
		mLinks.erase(mLinks.begin() + linkIndex);
	}

	void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
	{
		for (int a = 0; a < 2; a++)
		{
			std::string b = std::to_string(a);
			drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), b.c_str());

		}
	}

	const size_t GetTemplateCount() override
	{
		return sizeof(mTemplates) / sizeof(GraphEditor::Template);
	}

	const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
	{
		return mTemplates[index];
	}

	const size_t GetNodeCount() override
	{
		return mNodes.size();
	}

	const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
	{
		const auto& myNode = mNodes[index];
		return GraphEditor::Node
		{
			myNode.name,
			myNode.templateIndex,
			ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 200, myNode.y + 70)),
			myNode.mSelected
		};
	}

	const size_t GetLinkCount() override
	{
		return mLinks.size();
	}

	const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
	{
		return mLinks[index];
	}

	// Graph datas
	static const inline GraphEditor::Template mTemplates[] = {
		{
			IM_COL32(160, 160, 180, 255),
			IM_COL32(100, 100, 140, 255),
			IM_COL32(110, 110, 150, 255),
			1,
			Array{"Input"},
			nullptr,
			1,
			Array{"Output"},
			nullptr
		},

	};

	struct Node
	{
		const char* name;
		GraphEditor::TemplateIndex templateIndex;
		float x, y;
		bool mSelected;
	};

	std::vector<Node> mNodes = {
		{
			"My Node 0",
			0,
			0, 0,
			false
		},

		{
			"My Node 1",
			0,
			400, 0,
			false
		},

		{
			"My Node 2",
			0,
			400, 400,
			false
		}
	};

	std::vector<GraphEditor::Link> mLinks = { {0, 0, 1, 0} };
};

class EditorUI
{
private:
	std::shared_ptr<Truth::Managers> m_manager;
	// static int32 m_selectedEntity;
	int m_notUsedID;

	std::vector<const char*>& m_componentList;

	char inputTextBuffer[256];

	OPENFILENAME m_openFileName;
	TCHAR m_filePathBuffer[256] = L"";
	TCHAR m_fileBuffer[256] = L"";
	const TCHAR* m_sceneFileFilter = L"Scene 파일\0*.scene";
	const TCHAR* m_entityFileFilter = L"Entity 파일\0*.entity";

	HWND m_hwnd;

	std::queue<std::pair<std::weak_ptr<Truth::Entity>, int>> m_deletedComponent;
	std::queue<std::weak_ptr<Truth::Entity>> m_createdEntity;

	std::weak_ptr<Truth::Entity> m_selectedEntity;

	std::shared_ptr<Truth::AnimationInfo> m_selectedState;

public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager, HWND _hwnd);
	~EditorUI() {}
	void RenderUI(bool* p_open);

private:
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	void ShowContentsDrawerWindow(bool* p_open);
	void ShowMenuBar(bool* p_open);
	void ShowAnimator(bool* p_open);

	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);

	void AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity);

	void DisplayComponent(std::shared_ptr<Truth::Component> _component);

	void DisplayEntity(std::weak_ptr<Truth::Entity> _entity);

	//void DisplayAnimatorInspector(Node _animationInfo);


private:
	// 테스트용
	int m_testSize = 0;
	int m_selectedNum = 0;


};



//};
