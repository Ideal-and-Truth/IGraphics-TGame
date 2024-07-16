#pragma once
#include "Component.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "GraphEditor.h"

namespace Truth
{
	class AnimatorController;
	class SkinnedMesh;
}

namespace Truth
{
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
			m_animationInfos[nodeIndex].mSelected = selected;
		}

		void MoveSelectedNodes(const ImVec2 delta) override
		{
			for (auto& node : m_animationInfos)
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
			/// ProgressBar
			if (m_animationInfos[nodeIndex].isAnimationPlayed)
			{
				static float progress = 0.0f, progress_dir = 1.f;

				progress += progress_dir * 0.1f * ImGui::GetIO().DeltaTime;
				if (progress > +1.f) { progress = 1.f; progress_dir *= -1.0f; }
				if (progress < 0.f) { progress = 0.0f; progress_dir *= -1.0f; }

				ImVec2 lenVec(rectangle.Max.x - rectangle.Min.x, rectangle.Max.y - rectangle.Min.y);
				ImVec2 pVec;
				pVec.x = rectangle.Min.x + lenVec.x * progress;
				pVec.y = rectangle.Max.y - 30.f;
				drawList->AddRectFilled(rectangle.Min, pVec, IM_COL32(155, 155, 255, 255));
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
			return m_animationInfos.size();
		}

		const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
		{
			const auto& myNode = m_animationInfos[index];
			return GraphEditor::Node
			{
				myNode.name.c_str(),
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

		struct AnimatorNode
		{
			std::string name = "Empty";
			GraphEditor::TemplateIndex templateIndex = 0;
			float x = 0, y = 0;
			bool mSelected = false;

			bool isAnimationPlayed = false;
			std::wstring animationPath = L"";
			bool isChangesOnFinished = false;

			float animationSpeed = 1.f;
		};

		std::vector<AnimatorNode> m_animationInfos = {
			{
				"Empty",
				0,
				0, 0,
				false
			}
		};

		std::vector<GraphEditor::Link> mLinks = { {0, 0, 0, 0} };
	};


	class Animator :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Animator);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Truth::AnimatorController> m_animatorController;
		std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;

	public:
		Animator();
		virtual ~Animator();

		METHOD(Initalize);
		void Initalize();

		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(Update);
		void Update();


	};

	template<class Archive>
	void Truth::Animator::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

	template<class Archive>
	void Truth::Animator::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}
}



BOOST_CLASS_EXPORT_KEY(Truth::Animator)
BOOST_CLASS_VERSION(Truth::Animator, 0)