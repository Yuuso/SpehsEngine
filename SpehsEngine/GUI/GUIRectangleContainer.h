#pragma once
#include <vector>
#include <iostream>
#include "SpehsEngine/GUI/GUIRectangle.h"

namespace se
{
	class GUIWindow;
	class GUIRectangleScrollList;
	class GUIRectangleTree;

	/**Base class for GUI rectangle containers\n
	Each derived container must implement the following pure virtual methods:\n
	\tupdatePosition: how are the elements within the container positioned relative to container position?\n
	\tupdateScale: how are the elements inside the container scaled?\n
	\tupdateMinSize: what is the minimal space required for the container? Used by other containers when containers come in multiple layers.\n
	Container elements are stored in elements vector.\n
	The container automatically calls updates and renders elements within the index range from beginElementIndex up to updateElementCount.\n
	Therefore, beginElementIndex + updateElementCount should always be equal or less than total element count.
	*/
	class GUIRectangleContainer : public GUIRectangle
	{
	public:
		GUIRectangleContainer(GUIContext& context);
		~GUIRectangleContainer() override;

		void inputUpdate() override;
		void visualUpdate() override;
		void setRenderState(const bool _state) override;

		/*These pure virtual methods MUST be implemented by the derived class*/
		virtual void updateMinSize() = 0;
		virtual void updateScale() = 0;
		virtual void updatePosition() = 0;

		bool isReceivingInput() const override;
		virtual bool open();//Open container dimension
		virtual bool close();//Close container dimension
		virtual void toggleOpen();//Open/close container dimension
		void setDepth(const int16_t depth) override;

		//Element management
		virtual GUIRectangle* addElement(GUIRectangle* element);
		virtual bool moveElement(GUIRectangle* element, const size_t targetIndexBeforeMove);
		virtual bool removeElement(GUIRectangle* element);//Searches elements list and removes element. If element is not found, returns false
		virtual void queueElementRemoval(GUIRectangle& element) { queuedElementRemovals.push_back(&element); }
		virtual void processQueuedElementRemovals();
		virtual GUIRectangle* back() const { return elements.back(); }
		virtual GUIRectangle* front() const { return elements.front(); }
		GUIRectangle* at(const size_t index) const { if (index >= elements.size()) return nullptr; return elements[index]; }
		GUIRectangle* operator[](size_t index) const { return elements[index]; }
		/*Returns the size of the elements vector*/
		size_t elementsSize() const { return elements.size(); }
		virtual void clear();

		//Hierarchy
		bool isDescendant(GUIRectangle* element) const;
		
		//State
		void enableStateRecursive(const GUIRECT_STATE_TYPE stateBit) override;
		void disableStateRecursive(const GUIRECT_STATE_TYPE stateBit) override;
		
		////Templates
		//Closes all GUI rectangles except the specified exclude and all elements in a parent chain above it any of its parents
		template <typename T>
		void closeElementsOfType(GUIRectangleContainer* exclude = nullptr)
		{
			//This
			if (this == exclude)
				return;//DO NOT CLOSE THIS NOR ANYTHING BELOW
			if (dynamic_cast<T*>(this) && !exclude->isDescendantOf(this))
				close();

			//Elements
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (elements[i]->getAsGUIRectangleContainerPtr())
					elements[i]->getAsGUIRectangleContainerPtr()->closeElementsOfType<T>(exclude);
				else
				{
					T* elementAsTPtr(dynamic_cast<T*>(elements[i]));
					if (elementAsTPtr && elementAsTPtr != exclude && !exclude->isDescendantOf(elementAsTPtr))
						close();
				}
			}
		}

		//Identity
		GUIRectangleContainer* getAsGUIRectangleContainerPtr() override { return this; }

	protected:
		void onEnableInput() override;
		void onDisableInput() override;

		std::vector<GUIRectangle*> elements;
		std::vector<GUIRectangle*> queuedElementRemovals;

	};
}