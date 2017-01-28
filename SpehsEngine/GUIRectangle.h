#pragma once
#include <string>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "BitwiseOperations.h"
#include "TextureManager.h"//Used to get TextureParameter...
#define GUIRECT_ID_TYPE						uint32_t
#define TEXT_PREFERRED_SIZE_BORDER			2
////GUI rectangle states
#define GUIRECT_STATE_TYPE					uint32_t
#define GUIRECT_MOUSE_HOVER					0x00000001
#define GUIRECT_SCALE_UPDATED_BIT			0x00000002
#define GUIRECT_POSITION_UPDATED_BIT		0x00000004
#define GUIRECT_MIN_SIZE_UPDATED_BIT		0x00000008
#define GUIRECT_REMOVE_BIT					0x00000010//Does not actually remove element. Must have a higher level manager to monitor remove status and act accordingly to it
#define GUIRECT_MOUSE_HOVER_PREVIOUS		0x00000020
#define GUIRECT_UNUSED1						0x00000040
#define GUIRECT_UNUSED2						0x00000080
#define GUIRECT_UNUSED3						0x00000100
#define GUIRECT_UNUSED4						0x00000200
#define GUIRECT_UNUSED5						0x00000400
//GUI rectangle container
#define GUIRECT_MOUSE_HOVER_CONTAINER		0x00000800//Whether mouse hover has been detected inside an element of the container
#define GUIRECT_OPEN_BIT					0x00001000//Open containers update elements inside
//GUI window specific
#define GUIRECT_DRAGGING_BIT				0x00004000
#define GUIRECT_STRECHING_BIT				0x00008000
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
	namespace audio
	{
		class SoundSource;
	}
	class Text;
	class Polygon;
	class GUIWindow;
	class GUIButton;
	class GUICheckbox;
	class GUIStringEditor;
	class GUIRectangleRow;
	class GUIRectangleTree;
	class GUIRectangleScrollList;
	class GUIRectangleGrid;
	class GUIRectangleTable;
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
		friend class GUIRectangleContainer;
		friend class GUIWindow;

	public:
		GUIRectangle();
		GUIRectangle(const GUIRECT_ID_TYPE ID);
		GUIRectangle(const std::string str);
		GUIRectangle(const glm::ivec2& _size);
		GUIRectangle(const int width, const int height);
		virtual ~GUIRectangle();
		
		/// During GUI's input update the element's size and/or min size may change even so that it might affect parents above.
		virtual void inputUpdate();
		/// During GUI's visual update the element's size and/or min size must not change!
		virtual void visualUpdate();

		///TRANSFORM CHANGES: OVERRIDING METHOD MUST ENABLE THE CORRESPONDING GUIRECT_X_UPDATED_BIT OR CALL THE BASE METHOD
		/// Update min size is called up to once per update.
		virtual void updateMinSize();
		/**During scale update dimensions must not change in a manner that would affect parents above.\n
		scale update is called up to once per update if the size of the rectangle has changed*/
		virtual void updateScale();
		/// Positon update is called up to once per update if the position of the rectangle has changed
		virtual void updatePosition();

		virtual void setRenderState(const bool _state);
		virtual bool getRenderState() const;
		/// Checks whether the mouse is above this rectangle. Returns mouse hover value
		virtual bool updateMouseHover();
		/// Range [0.0f, 1.0f]
		void setColor(const glm::vec3& color);
		void setColor(const glm::vec4& color);
		/// Range [0, 255]
		void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void setColorAlpha(const float a);
		void setColorAlpha(const unsigned char a);
		void setParent(GUIRectangleContainer* Parent);
		virtual void setDepth(const int16_t depth);
		int16_t getDepth() const;
		spehs::Polygon* getPolygonPtr() const { return polygon; }
		//Hierarchy
		GUIRectangleContainer* getParentPtr() const { return parent; }
		GUIRectangleContainer* getFirstGenerationParent();
		bool isDescendantOf(GUIRectangleContainer* ascendant) const;
		//ID
		void setID(const int newID){ id = newID; }
		int getID() const { return id; }
		//Text
		virtual void setString(const std::string str);
		virtual void setStringSize(const int size);
		virtual void setStringSizeRelative(const int relativeSize);///< Set string size relative to global default GUI text size.
		virtual void setStringColor(const glm::vec4& col);
		virtual void setStringColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		virtual void setStringAlpha(const float alpha);
		virtual void setStringAlpha(const unsigned char a);
		std::string getString() const;
		virtual void setJustification(const GUIRECT_STATE_TYPE justificationBit);///<NOTE: if non-justification bit is given, all justification bits will be cleared and given bit will be enabled
		//Tooltip
		void setTooltip(const std::string tooltipString);
		GUIRectangle* getTooltipPtr() const { return tooltip; }
		//Display texture
		void setDisplayTexture(const std::string path, const TextureParameter& _parameters);
		void setDisplayTexture(const std::string path);
		//Rectangle texture
		void setTexture(const std::string path, const TextureParameter& _parameters);
		void setTexture(const std::string path);
		void setTextureID(const unsigned int _textureID);
		
		//Audio
		void setHoverSound(const std::string path);
		void setPressSound(const std::string path);
		
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
		GUIRECT_STATE_TYPE getState() const { return state; }
		bool checkState(GUIRECT_STATE_TYPE bit) const { return checkBit(state, bit); }
		//"Shortcut" getters
		bool getMouseHover() const { return checkBit(state, GUIRECT_MOUSE_HOVER); }
		bool getMouseHoverContainer() const { return checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		bool getMouseHoverAny() const { return checkBit(state, GUIRECT_MOUSE_HOVER) | checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER); }
		virtual bool isReceivingInput() const { return inputEnabled && checkBit(state, GUIRECT_RECEIVING_INPUT); }
		virtual bool isSelected() const { return checkBit(state, GUIRECT_SELECTED); }
		virtual bool isOpen() const { return checkBit(state, GUIRECT_OPEN_BIT); }
		//Setters
		virtual void enableStateRecursive(const GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		virtual void disableStateRecursive(const GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		void enableStateRecursiveUpwards(const GUIRECT_STATE_TYPE stateBit);
		void disableStateRecursiveUpwards(const GUIRECT_STATE_TYPE stateBit);
		virtual void enableState(const GUIRECT_STATE_TYPE stateBit){ enableBit(state, stateBit); }
		virtual void disableState(const GUIRECT_STATE_TYPE stateBit){ disableBit(state, stateBit); }
		virtual void toggleState(const GUIRECT_STATE_TYPE stateBit){ toggleBit(state, stateBit); }

		//Enabling
		void enableInput(){ if (inputEnabled) return; onEnableInput(); }
		void disableInput(){ if (!inputEnabled) return; onDisableInput(); }
		bool getInputEnabled() const { return inputEnabled; }
		void setInputEnabled(const bool _state){ if (_state) enableInput(); else disableInput(); }
		
		////Managing element position
		//Setting both coordinates
		virtual void setPositionLocal(const glm::ivec2& pos){ setXLocal(pos.x); setYLocal(pos.y); }
		virtual void setPositionLocal(const int x, const int y){ setXLocal(x); setYLocal(y); }
		virtual void setPositionGlobal(const glm::ivec2& pos){ setXLocal(getXLocal() + pos.x - getXGlobal()); setYLocal(getYLocal() + pos.y - getYGlobal()); }
		virtual void setPositionGlobal(const int x, const int y){ setXLocal(getXLocal() + x - getXGlobal()); setYLocal(getYLocal() + y - getYGlobal()); }
		virtual void translate(const glm::ivec2& translation){ setXLocal(position.x + translation.x); setYLocal(position.y + translation.y); }
		virtual void translate(const int x, const int y){ setXLocal(position.x + x); setYLocal(position.y + y); }
		//Setting only one coordinate
		virtual void incrementX(const int incrementation){ setXLocal(position.x + incrementation); }
		virtual void incrementY(const int incrementation){ setYLocal(position.y + incrementation); }
		virtual void setXLocal(const int x){ position.x = x; disableBit(state, GUIRECT_POSITION_UPDATED_BIT); }
		virtual void setYLocal(const int y){ position.y = y; disableBit(state, GUIRECT_POSITION_UPDATED_BIT); }
		//Getting the GUIRectangle screen position (global)
		glm::ivec2 getPositionGlobal() const;
		int getXGlobal() const;
		int getYGlobal() const;
		//Local position getters
		glm::ivec2 getPositionLocal() const { return position; }
		int getXLocal() const { return position.x; }
		int getYLocal() const { return position.y; }

		//Managing element scale
		//Scale managament
		virtual void setSize(const int width, const int height){ setWidth(width); setHeight(height); }
		virtual void setSize(const glm::ivec2& newSize){ setWidth(newSize.x); setHeight(newSize.y); }
		virtual void setWidth(const int width);
		virtual void setHeight(const int height);
		//Getters
		virtual glm::ivec2 getSize(){ if (!(state & GUIRECT_SCALE_UPDATED_BIT)) updateScale(); return size; }
		virtual int getWidth(){ if (!(state & GUIRECT_SCALE_UPDATED_BIT)) updateScale(); return size.x; }
		virtual int getHeight(){ if (!(state & GUIRECT_SCALE_UPDATED_BIT)) updateScale(); return size.y; }
		virtual glm::ivec2 getMinSize(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED_BIT)) updateMinSize(); return minSize; }
		virtual int getMinWidth(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED_BIT)) updateMinSize(); return minSize.x; }
		virtual int getMinHeight(){ if (!(state & GUIRECT_MIN_SIZE_UPDATED_BIT)) updateMinSize(); return minSize.y; }
		
		//Identity
		virtual GUIWindow* getAsGUIWindowPtr() { return nullptr; }
		virtual GUICheckbox* getAsGUICheckboxPtr() { return nullptr; }
		virtual GUIStringEditor* getAsGUIStringEditorPtr() { return nullptr; }
		virtual GUIRectangleRow* getAsGUIRectangleRowPtr() { return nullptr; }
		virtual GUIRectangleTree* getAsGUIRectangleTreePtr() { return nullptr; }
		virtual GUIRectangleScrollList* getAsGUIRectangleScrollListPtr() { return nullptr; }
		virtual GUIRectangleGrid* getAsGUIRectangleGridPtr() { return nullptr; }
		virtual GUIRectangleTable* getAsGUIRectangleTablePtr() { return nullptr; }
		virtual GUIRectangleContainer* getAsGUIRectangleContainerPtr() { return nullptr; }


	protected:
		void createText();
		virtual void onEnableInput(){ inputEnabled = true; }
		virtual void onDisableInput(){ inputEnabled = false; }

		glm::vec4 color;///<Color values given to polygon. Ranges from 0.0f - 1.0f
		glm::ivec2 position;///<The position of the rectangle, originating from the lower left corner, given in screen coordinates. Relative to parent's position
		glm::ivec2 size;///<Current size of the rectangle
		glm::ivec2 minSize;///<The minimum size of the rectangle. Checked whenever rezising the polygon.
		GUIRectangleContainer* parent;///<Rectangle inherits position from parent chain. NOTE: parent must be ractangle container
		GUIRectangle* tooltip;
		Polygon* polygon;
		Text* text;
		spehs::audio::SoundSource* hoverSound;
		spehs::audio::SoundSource* pressSound;
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
		bool inputEnabled;
	};
}