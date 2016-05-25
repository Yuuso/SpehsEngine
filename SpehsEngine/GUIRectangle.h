#pragma once
#include <string>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "BitwiseOperations.h"
#define GUIRECT_ID_TYPE						uint32_t
#define TEXT_PREFERRED_SIZE_BORDER			2
////GUI rectangle states
#define GUIRECT_STATE_TYPE					uint32_t
#define GUIRECT_MOUSE_HOVER					0x00000001
#define GUIRECT_SCALED						0x00000002
#define GUIRECT_POSITIONED					0x00000004
#define GUIRECT_ENABLED						0x00000008
#define GUIRECT_MIN_SIZE_UPDATED			0x00000010
#define GUIRECT_UNUSED0						0x00000020
#define GUIRECT_UNUSED1						0x00000040
#define GUIRECT_UNUSED2						0x00000080
#define GUIRECT_UNUSED3						0x00000100
#define GUIRECT_UNUSED4						0x00000200
#define GUIRECT_UNUSED5						0x00000400
//GUI rectangle container
#define GUIRECT_MOUSE_HOVER_CONTAINER		0x00000800//Whether mouse hover has been detected inside an element of the container
#define GUIRECT_OPEN						0x00001000//Different containers have different meanings for being open
//GUI window specific
#define GUIRECT_FOCUSED						0x00002000
#define GUIRECT_DRAGGING					0x00004000
#define GUIRECT_STRECHING					0x00008000
//Text justification
#define GUIRECT_TEXT_JUSTIFICATION_LEFT		0x00010000
#define GUIRECT_TEXT_JUSTIFICATION_CENTER	0x00020000
#define GUIRECT_TEXT_JUSTIFICATION_RIGHT	0x00040000
////Misc
#define GUIRECT_RECEIVING_INPUT				0x00080000//Underlings should inform their first generation parents when receiving input
#define GUIRECT_SELECTED					0x00100000
#define GUIRECT_HOVER_COLOR					0x00200000
#define DRAW_TEXT_ENABLED

namespace spehs
{
	class Text;
	class Polygon;
	class GUIWindow;
	class GUIButton;
	class GUICheckbox;
	class GUITextField;
	class GUIRectangleRow;
	class GUIRectangleTree;
	class GUIRectangleList;
	class GUIRectangleGrid;
	class GUIRectangleContainer;

	/**Base class for every GUI element\n\n
	All GUI elements are expected to be derived from this class.\n
	GUI rectangles can be stored in GUI rectangle containers*/
	class GUIRectangle
	{
	////Static
	public:
		static void setDefaultColor(int r, int g, int b, int a = 255);
		static void setDefaultColor(glm::vec4 color);
		static void setDefaultColor(glm::vec3 color);
	private:
		static glm::vec4 defaultColor;


	public:
		GUIRectangle();
		GUIRectangle(GUIRECT_ID_TYPE ID);
		GUIRectangle(std::string str);
		GUIRectangle(glm::ivec2& _size);
		GUIRectangle(int width, int height);
		virtual ~GUIRectangle();
		friend class GUIWindow;

#ifdef DRAW_TEXT_ENABLED
		//DEBUG
		virtual void DRAW_TEXT();
#endif

		///During GUI's update the element's size and/or min size may change even so that it might affect parents above.
		virtual void update();
		virtual void postUpdate();
		/**During scale update dimensions must not change in a manner that would affect parents above.\n
		scale update is called up to once per update if the size of the rectangle has changed*/
		virtual void updateScale();
		///Positon update is called up to once per update if the position of the rectangle has changed
		virtual void updatePosition();
		virtual void setRenderState(const bool _state);		
		virtual void updateMinSize();
		/*Checks whether the mouse is above this rectangle. Returns mouse hover value*/
		virtual bool updateMouseHover();
		/*Range [0.0f, 1.0f]*/
		void setColor(glm::vec4& color);
		/*Range [0, 255]*/
		void setColor(int r, int g, int b, int a = 255);
		void setParent(GUIRectangle* Parent);
		virtual void setDepth(uint16_t depth);
		uint16_t getDepth();
		spehs::Polygon* getPolygonPtr(){ return polygon; }
		GUIRectangle* getParentPtr(){ return parent; }
		GUIRectangle* getFirstGenerationParent();
		//ID
		void setID(int newID){ id = newID; }
		int getID(){ return id; }
		//Text
		void createText();
		virtual void setString(std::string str);
		void setStringSize(int size);
		void setStringSizeRelative(int relativeSize);///< Set string size relative to global default GUI text size.
		void setStringColor(glm::vec4& col);
		void setStringColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		void setStringAlpha(float alpha);
		void setStringAlpha(unsigned char a);
		std::string getString();
		void setJustification(GUIRECT_STATE_TYPE justificationBit);///<NOTE: if non-justification bit is given, all justification bits will be cleared and given bit will be enabled
		//Tooltip
		void setTooltip(std::string tooltipString);
		GUIRectangle* getTooltipPtr(){ return tooltip; }
		//Display texture
		void setDisplayTexture(std::string path);
		//Rectangle texture
		void setTexture(std::string path);
		
		//Callback
		/** Callback to function/method that returns void and accepts the button reference as an argument.
		Called when the rectangle is left pressed.
		IMPORTANT NOTE: callback function must not remove the element, as the callback is called from the update.*/
		void setPressCallback(std::function<void(GUIRectangle&)> callbackFunction);

		////State
		//Getters
		GUIRECT_STATE_TYPE getState(){ return state; }
		bool checkState(GUIRECT_STATE_TYPE bit){ return checkBit(state, bit); }
		//"Shortcut" getters
		bool getMouseHover(){ return checkBit(state, GUIRECT_MOUSE_HOVER); }
		bool getMouseHoverContainer(){ return checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		bool getMouseHoverAny(){ if (checkBit(state, GUIRECT_MOUSE_HOVER)) return true; return checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		bool isEnabled(){ return checkBit(state, GUIRECT_ENABLED); }
		bool isFocused(){ return checkBit(state, GUIRECT_FOCUSED); }
		bool isReceivingInput(){ return checkBit(state, GUIRECT_RECEIVING_INPUT); }
		bool isStreching(){ return checkBit(state, GUIRECT_STRECHING); }
		bool isDragging(){ return checkBit(state, GUIRECT_DRAGGING); }
		bool isSelected(){ return checkBit(state, GUIRECT_SELECTED); }
		bool isOpen(){ return checkBit(state, GUIRECT_OPEN); }
		bool isVisible();
		//Setters
		virtual void enableStateRecursive(GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		virtual void disableStateRecursive(GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		void enableStateRecursiveUpwards(GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); if (parent) parent->enableStateRecursiveUpwards(stateBit); }
		void disableStateRecursiveUpwards(GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); if (parent) parent->disableStateRecursiveUpwards(stateBit); }
		void enableState(GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		void disableState(GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		void toggleState(GUIRECT_STATE_TYPE stateBit){ toggleBit(state, stateBit); }
		virtual void loseFocus(){ disableBit(state, GUIRECT_FOCUSED); }


		////Managing element position
		//Setting both coordinates
		virtual void setPosition(glm::ivec2& pos){ setX(pos.x); setY(pos.y); }
		virtual void setPosition(int x, int y){ setX(x); setY(y); }
		virtual void translate(glm::ivec2& translation){ setX(position.x + translation.x); setY(position.y + translation.y); }
		virtual void translate(int x, int y){ setX(position.x + x); setY(position.y + y); }
		//Setting only one coordinate
		virtual void setX(int x){ position.x = x; disableBit(state, GUIRECT_POSITIONED); }
		virtual void setY(int y){ position.y = y; disableBit(state, GUIRECT_POSITIONED); }
		virtual void incrementX(int incrementation){ setX(position.x + incrementation); }
		virtual void incrementY(int incrementation){ setY(position.y + incrementation); }
		//Global screen position getters
		glm::ivec2 getPosition(){ if (parent) return parent->getPosition() + position; return position; }
		int getX(){ if (parent) return parent->getX() + position.x; return position.x; }
		int getY(){ if (parent) return parent->getY() + position.y; return position.y; }
		//Local position getters
		glm::ivec2 getLocalPosition(){ return position; }
		int getLocalX(){ return position.x; }
		int getLocalY(){ return position.y; }
		//Scale managament
		virtual void setSize(int width, int height){ setWidth(width); setHeight(height); }
		virtual void setSize(glm::ivec2& newSize){ setWidth(newSize.x); setHeight(newSize.y); }
		virtual void setWidth(int width){ if (size.x == width) return; size.x = width; disableStateRecursiveUpwards(GUIRECT_SCALED); }
		virtual void setHeight(int height){ if (size.y == height) return; size.y = height; disableStateRecursiveUpwards(GUIRECT_SCALED); }
		//Getters
		virtual glm::ivec2 getSize(){ return size; }
		virtual int getWidth(){ return size.x; }
		virtual int getHeight(){ return size.y; }
		virtual glm::ivec2 getMinSize(){ return minSize; }
		virtual int getMinWidth(){ return minSize.x; }
		virtual int getMinHeight(){ return minSize.y; }


		//Identity
		GUIRectangle* getAsGUIRectanglePtr(){ return this; }
		virtual GUIWindow* getAsGUIWindowPtr(){ return nullptr; }
		virtual GUICheckbox* getAsGUICheckboxPtr(){ return nullptr; }
		virtual GUITextField* getAsGUITextFieldPtr(){ return nullptr; }
		virtual GUIRectangleRow* getAsGUIRectangleRowPtr(){ return nullptr; }
		virtual GUIRectangleTree* getAsGUIRectangleTreePtr(){ return nullptr; }
		virtual GUIRectangleList* getAsGUIRectangleListPtr(){ return nullptr; }
		virtual GUIRectangleGrid* getAsGUIRectangleGridPtr(){ return nullptr; }
		virtual GUIRectangleContainer* getAsGUIRectangleContainerPtr(){ return nullptr; }

	protected:
		glm::vec4 color;///<Color values given to polygon. Ranges from 0.0f - 1.0f
		glm::ivec2 position;///<The position of the rectangle, originating from the lower left corner, given in screen coordinates. Relative to parent's position
		glm::ivec2 size;///<Current size of the rectangle
		glm::ivec2 minSize;///<The minimum size of the rectangle. Checked whenever rezising the polygon.
		GUIRectangle* parent;///<Rectangle inherits position from parent chain. NOTE: parent must be ractangle container
		Polygon* polygon;
		GUIRectangle* tooltip;
		Text* text;
		GUIRECT_STATE_TYPE state;
		GUIRECT_ID_TYPE id;///<GUI rectangles can be given IDs for identification
		std::function<void(GUIRectangle&)>* pressCallbackFunction;//Called when pressed

		struct DisplayTexture
		{
			DisplayTexture() : polygon(nullptr), width(0), height(0){}
			~DisplayTexture();
			Polygon* polygon;
			uint16_t width;
			uint16_t height;
		};
		DisplayTexture* displayTexture;

	private:

	};
}