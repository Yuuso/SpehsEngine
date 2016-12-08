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
#define GUIRECT_MIN_SIZE_UPDATED			0x00000008
#define GUIRECT_ENABLED_BIT					0x00000010//Enabled user interaction
#define GUIRECT_REMOVE_BIT					0x00000020//Does not actually remove element. Must have a higher level manager to monitor remove status and act accordingly to it
#define GUIRECT_UNUSED1						0x00000040
#define GUIRECT_UNUSED2						0x00000080
#define GUIRECT_UNUSED3						0x00000100
#define GUIRECT_UNUSED4						0x00000200
#define GUIRECT_UNUSED5						0x00000400
//GUI rectangle container
#define GUIRECT_MOUSE_HOVER_CONTAINER		0x00000800//Whether mouse hover has been detected inside an element of the container
#define GUIRECT_OPEN						0x00001000//Different containers have different meanings for being open
//GUI window specific
#define GUIRECT_DRAGGING_BIT				0x00004000
#define GUIRECT_STRECHING_BIT				0x00008000
#define GUIRECT_REFRESH_BIT					0x00010000//Prevents multiple refreshes within a single update cycle. Checked at GUIWindow post update. Make refreshing the window within element update possible, otherwise resfresh would deallocate the element being updated!
//Text justification
#define GUIRECT_TEXT_JUSTIFICATION_LEFT		0x00020000
#define GUIRECT_TEXT_JUSTIFICATION_CENTER	0x00040000
#define GUIRECT_TEXT_JUSTIFICATION_RIGHT	0x00080000
////Misc
#define GUIRECT_RECEIVING_INPUT				0x00100000//Underlings should inform their first generation parents when receiving input
#define GUIRECT_SELECTED					0x00200000
#define GUIRECT_HOVER_COLOR					0x00400000//If enabled, rectangle will highlight when under mouse

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
		static int16_t defaultDepth;//Default depth where GUI rectangles will be arranged
		static int16_t tooltipDepthRelative;//Default depth modifier for tooltips relative to their parent GUI rectangle
		static glm::vec4 defaultColor;//Newly created GUI rectangles will have this color by default
		static glm::vec4 defaultStringColor;//Newly created GUI rectangle strings will have this color by default
		static glm::vec4 defaultTooltipColor;//Newly created tooltips will have this color by default
		static glm::vec4 defaultTooltipStringColor;//Newly created tooltip string will have this color by default
	private:

	public:
		GUIRectangle();
		GUIRectangle(GUIRECT_ID_TYPE ID);
		GUIRectangle(std::string str);
		GUIRectangle(glm::ivec2& _size);
		GUIRectangle(int width, int height);
		virtual ~GUIRectangle();
		friend class GUIWindow;
		
		/// During GUI's update the element's size and/or min size may change even so that it might affect parents above.
		virtual void update();
		virtual void postUpdate();
		/**During scale update dimensions must not change in a manner that would affect parents above.\n
		scale update is called up to once per update if the size of the rectangle has changed*/
		virtual void updateScale();
		/// Positon update is called up to once per update if the position of the rectangle has changed
		virtual void updatePosition();
		virtual void setRenderState(const bool _state);
		virtual bool getRenderState();
		virtual void updateMinSize();
		/// Checks whether the mouse is above this rectangle. Returns mouse hover value
		virtual bool updateMouseHover();
		/// Range [0.0f, 1.0f]
		void setColor(glm::vec3& color);
		void setColor(glm::vec4& color);
		/// Range [0, 255]
		void setColor(int r, int g, int b, int a = 255);
		void setParent(GUIRectangleContainer* Parent);
		virtual void setDepth(int16_t depth);
		int16_t getDepth();
		spehs::Polygon* getPolygonPtr(){ return polygon; }
		GUIRectangleContainer* getParentPtr(){ return parent; }
		GUIRectangleContainer* getFirstGenerationParent();
		//ID
		void setID(int newID){ id = newID; }
		int getID(){ return id; }
		//Text
		virtual void setString(std::string str);
		void setStringSize(int size);
		void setStringSizeRelative(int relativeSize);///< Set string size relative to global default GUI text size.
		void setStringColor(glm::vec4& col);
		void setStringColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		void setStringAlpha(float alpha);
		void setStringAlpha(unsigned char a);
		std::string getString();
		virtual void setJustification(GUIRECT_STATE_TYPE justificationBit);///<NOTE: if non-justification bit is given, all justification bits will be cleared and given bit will be enabled
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
		IMPORTANT NOTE: callback function must not remove the element, as the callback is called from the update.
		
		Example use:
		#include <functional>
		guirect->setPressCallback(std::bind(&ShipEditor::methodTaking1Argument, ShipEditor::instancePtr, std::placeholders::_1));
		*/
		void setPressCallback(std::function<void(GUIRectangle&)> callbackFunction);

		////State
		//Getters
		GUIRECT_STATE_TYPE getState(){ return state; }
		bool checkState(GUIRECT_STATE_TYPE bit){ return checkBit(state, bit); }
		//"Shortcut" getters
		bool getMouseHover(){ return checkBit(state, GUIRECT_MOUSE_HOVER); }
		bool getMouseHoverContainer(){ return checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		bool getMouseHoverAny(){ return checkBit(state, GUIRECT_MOUSE_HOVER) | checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		virtual bool isReceivingInput(){ return checkBit(state, GUIRECT_RECEIVING_INPUT); }
		bool isSelected(){ return checkBit(state, GUIRECT_SELECTED); }
		bool isOpen(){ return checkBit(state, GUIRECT_OPEN); }
		//Setters
		virtual void enableStateRecursive(GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		virtual void disableStateRecursive(GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		void enableStateRecursiveUpwards(GUIRECT_STATE_TYPE stateBit);
		void disableStateRecursiveUpwards(GUIRECT_STATE_TYPE stateBit);
		void enableState(GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		void disableState(GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		void toggleState(GUIRECT_STATE_TYPE stateBit){ toggleBit(state, stateBit); }

		//Enabling
		void enable(){ if (!checkState(GUIRECT_ENABLED_BIT)) onEnable(); }
		void disable(){ if (checkState(GUIRECT_ENABLED_BIT)) onDisable(); }
		virtual void onEnable(){ enableState(GUIRECT_ENABLED_BIT); }
		virtual void onDisable(){ disableState(GUIRECT_ENABLED_BIT); }
		
		////Managing element position
		//Setting both coordinates
		virtual void setPositionLocal(glm::ivec2& pos){ setXLocal(pos.x); setYLocal(pos.y); }
		virtual void setPositionLocal(int x, int y){ setXLocal(x); setYLocal(y); }
		virtual void setPositionGlobal(glm::ivec2& pos){ setXLocal(getXLocal() + pos.x - getXGlobal()); setYLocal(getYLocal() + pos.y - getYGlobal()); }
		virtual void setPositionGlobal(int x, int y){ setXLocal(getXLocal() + x - getXGlobal()); setYLocal(getYLocal() + y - getYGlobal()); }
		virtual void translate(glm::ivec2& translation){ setXLocal(position.x + translation.x); setYLocal(position.y + translation.y); }
		virtual void translate(int x, int y){ setXLocal(position.x + x); setYLocal(position.y + y); }
		//Setting only one coordinate
		virtual void incrementX(int incrementation){ setXLocal(position.x + incrementation); }
		virtual void incrementY(int incrementation){ setYLocal(position.y + incrementation); }
		virtual void setXLocal(int x){ position.x = x; disableBit(state, GUIRECT_POSITIONED); }
		virtual void setYLocal(int y){ position.y = y; disableBit(state, GUIRECT_POSITIONED); }
		//Getting the GUIRectangle screen position (global)
		glm::ivec2 getPositionGlobal();
		int getXGlobal();
		int getYGlobal();
		//Local position getters
		glm::ivec2 getPositionLocal(){ return position; }
		int getXLocal(){ return position.x; }
		int getYLocal(){ return position.y; }

		//Managing element scale
		//Scale managament
		virtual void setSize(int width, int height){ setWidth(width); setHeight(height); }
		virtual void setSize(glm::ivec2& newSize){ setWidth(newSize.x); setHeight(newSize.y); }
		virtual void setWidth(int width){ if (size.x == width) return; size.x = width; disableStateRecursiveUpwards(GUIRECT_SCALED); }
		virtual void setHeight(int height){ if (size.y == height) return; size.y = height; disableStateRecursiveUpwards(GUIRECT_SCALED); }
		//Getters
		virtual glm::ivec2 getSize(){ if (!(state & GUIRECT_SCALED)) updateScale(); return size; }
		virtual int getWidth(){ if (!(state & GUIRECT_SCALED)) updateScale(); return size.x; }
		virtual int getHeight(){ if (!(state & GUIRECT_SCALED)) updateScale(); return size.y; }
		virtual glm::ivec2 getMinSize(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED)) updateMinSize(); return minSize; }
		virtual int getMinWidth(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED)) updateMinSize(); return minSize.x; }
		virtual int getMinHeight(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED)) updateMinSize(); return minSize.y; }
		
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
		void createText();

		glm::vec4 color;///<Color values given to polygon. Ranges from 0.0f - 1.0f
		glm::ivec2 position;///<The position of the rectangle, originating from the lower left corner, given in screen coordinates. Relative to parent's position
		glm::ivec2 size;///<Current size of the rectangle
		glm::ivec2 minSize;///<The minimum size of the rectangle. Checked whenever rezising the polygon.
		GUIRectangleContainer* parent;///<Rectangle inherits position from parent chain. NOTE: parent must be ractangle container
		GUIRectangle* tooltip;
		Polygon* polygon;
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