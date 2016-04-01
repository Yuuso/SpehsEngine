#pragma once
#include <vector>
#include "GUIRectangle.h"

namespace spehs
{
	class GUIWindow;
	class GUIRectangleList;
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
		GUIRectangleContainer();
		virtual ~GUIRectangleContainer();

		virtual void update();
		virtual void postUpdate();
		virtual void setRenderState(const bool _state);

		/*These pure virtual methods MUST be implemented by the derived class*/
		virtual void updatePosition() = 0;
		virtual void updateScale() = 0;
		virtual void updateMinSize() = 0;

		/*Returns true if the list has an textfield element underneath it that is currently active*/
		bool isReceivingTextInput();
		virtual void open();//Open container dimension
		virtual void close();//Close container dimension
		void closeTreeElements();//Closes every gui rectangle tree beneath this.
		void closeTreeElements(GUIRectangle* exclude);//Closes all other trees except select excluded
		virtual void setDepth(uint16_t depth);

		//Element management
		virtual void addElement(GUIRectangle* element);
		bool removeElement(GUIRectangle* element);//Searches elements list and removes element. If element is not found, returns false
		virtual GUIRectangle* back(){ return elements.back(); }
		GUIRectangle* at(int index){ if (index < 0 || index >= elements.size()) return nullptr; return elements[index]; }
		GUIRectangle* operator[](int index){ return elements[index]; }
		/*Returns the size of the elements vector*/
		unsigned elementsSize(){ return elements.size(); }
		virtual void clear();
		/*Returns index to the first visible element on the element list*/
		int getBeginElementIndex(){ return beginElementIndex; }
		/*Returns index to the last visible element on the list*/
		int getEndElementIndex();
		int getUpdateElementCount(){ return updateElementCount; }
		virtual void incrementUpdateElementCount(int incrementation);
		bool invisibleElements(){ if (elements.size() > updateElementCount) return true; return false; }
		void scroll(int amount);

		//State
		virtual void enableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		virtual void disableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		virtual void loseFocus();

		//Identity
		GUIRectangleContainer* getAsGUIRectangleContainerPtr(){ return this; }

	protected:
		std::vector<GUIRectangle*> elements;
		glm::ivec2 elementSize;///<Current size of a single element, if uniform size exists
		glm::ivec2 minElementSize;///<The size fitted to the largest min-size dimensions so that every element fits
		int beginElementIndex;///<Index of the first updated/rendered element
		int updateElementCount;///<Amount of elements updated/rendered beginning from begin element index

	};
}