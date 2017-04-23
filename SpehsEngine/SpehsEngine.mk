##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=SpehsEngine
ConfigurationName      :=Debug
WorkspacePath          := "/home/pi/projects/SpehsRebuut"
ProjectPath            := "/home/pi/projects/SpehsEngine/SpehsEngine"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=04/05/17
CodeLitePath           :="/home/pi/.codelite"
LinkerName             :=/usr/bin/g++ 
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="SpehsEngine.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/home/pi/projects/SpehsEngine/Dependencies/include $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++ 
CC       := /usr/bin/gcc 
CXXFLAGS := -std=c++11  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/AnimatedSprite.cpp$(ObjectSuffix) $(IntermediateDirectory)/ApplicationData.cpp$(ObjectSuffix) $(IntermediateDirectory)/Arrow.cpp$(ObjectSuffix) $(IntermediateDirectory)/AudioEngine.cpp$(ObjectSuffix) $(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Batch.cpp$(ObjectSuffix) $(IntermediateDirectory)/BatchManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Camera2D.cpp$(ObjectSuffix) $(IntermediateDirectory)/Collider.cpp$(ObjectSuffix) $(IntermediateDirectory)/Component.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Compression.cpp$(ObjectSuffix) $(IntermediateDirectory)/Console.cpp$(ObjectSuffix) $(IntermediateDirectory)/Exceptions.cpp$(ObjectSuffix) $(IntermediateDirectory)/FileStream.cpp$(ObjectSuffix) $(IntermediateDirectory)/GLSLProgram.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUICheckbox.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIColorEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUID.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIPopup.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangle.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GUIRectangleColumn.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangleContainer.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangleRow.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangleScrollList.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangleTable.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIRectangleTree.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIStringEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/GUIWindowManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Geometry.cpp$(ObjectSuffix) $(IntermediateDirectory)/InputManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/KeyboardRecorder.cpp$(ObjectSuffix) $(IntermediateDirectory)/LengthConversion.cpp$(ObjectSuffix) $(IntermediateDirectory)/Line.cpp$(ObjectSuffix) $(IntermediateDirectory)/OS.cpp$(ObjectSuffix) $(IntermediateDirectory)/OpenALError.cpp$(ObjectSuffix) $(IntermediateDirectory)/OpenGLError.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsWorld2D.cpp$(ObjectSuffix) $(IntermediateDirectory)/Point.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Polygon.cpp$(ObjectSuffix) $(IntermediateDirectory)/Position64.cpp$(ObjectSuffix) $(IntermediateDirectory)/PostProcessing.cpp$(ObjectSuffix) $(IntermediateDirectory)/Primitive.cpp$(ObjectSuffix) $(IntermediateDirectory)/RNG.cpp$(ObjectSuffix) $(IntermediateDirectory)/RigidBody2D.cpp$(ObjectSuffix) $(IntermediateDirectory)/SATCollision.cpp$(ObjectSuffix) $(IntermediateDirectory)/Serializable.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerializableDirectory.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerializableString.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/ShaderManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Shapes.cpp$(ObjectSuffix) $(IntermediateDirectory)/SoundSource.cpp$(ObjectSuffix) $(IntermediateDirectory)/SpehsEngine.cpp$(ObjectSuffix) $(IntermediateDirectory)/Sprite.cpp$(ObjectSuffix) $(IntermediateDirectory)/StringOperations.cpp$(ObjectSuffix) $(IntermediateDirectory)/Text.cpp$(ObjectSuffix) $(IntermediateDirectory)/Textbox.cpp$(ObjectSuffix) $(IntermediateDirectory)/TextureManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Thread.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Time.cpp$(ObjectSuffix) $(IntermediateDirectory)/Transform2D.cpp$(ObjectSuffix) $(IntermediateDirectory)/Triangulation.cpp$(ObjectSuffix) $(IntermediateDirectory)/Vertex.cpp$(ObjectSuffix) $(IntermediateDirectory)/Window.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/AnimatedSprite.cpp$(ObjectSuffix): AnimatedSprite.cpp $(IntermediateDirectory)/AnimatedSprite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/AnimatedSprite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AnimatedSprite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AnimatedSprite.cpp$(DependSuffix): AnimatedSprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AnimatedSprite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/AnimatedSprite.cpp$(DependSuffix) -MM "AnimatedSprite.cpp"

$(IntermediateDirectory)/AnimatedSprite.cpp$(PreprocessSuffix): AnimatedSprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AnimatedSprite.cpp$(PreprocessSuffix) "AnimatedSprite.cpp"

$(IntermediateDirectory)/ApplicationData.cpp$(ObjectSuffix): ApplicationData.cpp $(IntermediateDirectory)/ApplicationData.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/ApplicationData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ApplicationData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ApplicationData.cpp$(DependSuffix): ApplicationData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ApplicationData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ApplicationData.cpp$(DependSuffix) -MM "ApplicationData.cpp"

$(IntermediateDirectory)/ApplicationData.cpp$(PreprocessSuffix): ApplicationData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ApplicationData.cpp$(PreprocessSuffix) "ApplicationData.cpp"

$(IntermediateDirectory)/Arrow.cpp$(ObjectSuffix): Arrow.cpp $(IntermediateDirectory)/Arrow.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Arrow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Arrow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Arrow.cpp$(DependSuffix): Arrow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Arrow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Arrow.cpp$(DependSuffix) -MM "Arrow.cpp"

$(IntermediateDirectory)/Arrow.cpp$(PreprocessSuffix): Arrow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Arrow.cpp$(PreprocessSuffix) "Arrow.cpp"

$(IntermediateDirectory)/AudioEngine.cpp$(ObjectSuffix): AudioEngine.cpp $(IntermediateDirectory)/AudioEngine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/AudioEngine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AudioEngine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AudioEngine.cpp$(DependSuffix): AudioEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AudioEngine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/AudioEngine.cpp$(DependSuffix) -MM "AudioEngine.cpp"

$(IntermediateDirectory)/AudioEngine.cpp$(PreprocessSuffix): AudioEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AudioEngine.cpp$(PreprocessSuffix) "AudioEngine.cpp"

$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix): AudioManager.cpp $(IntermediateDirectory)/AudioManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/AudioManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AudioManager.cpp$(DependSuffix): AudioManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/AudioManager.cpp$(DependSuffix) -MM "AudioManager.cpp"

$(IntermediateDirectory)/AudioManager.cpp$(PreprocessSuffix): AudioManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AudioManager.cpp$(PreprocessSuffix) "AudioManager.cpp"

$(IntermediateDirectory)/Batch.cpp$(ObjectSuffix): Batch.cpp $(IntermediateDirectory)/Batch.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Batch.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Batch.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Batch.cpp$(DependSuffix): Batch.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Batch.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Batch.cpp$(DependSuffix) -MM "Batch.cpp"

$(IntermediateDirectory)/Batch.cpp$(PreprocessSuffix): Batch.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Batch.cpp$(PreprocessSuffix) "Batch.cpp"

$(IntermediateDirectory)/BatchManager.cpp$(ObjectSuffix): BatchManager.cpp $(IntermediateDirectory)/BatchManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/BatchManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BatchManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BatchManager.cpp$(DependSuffix): BatchManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BatchManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BatchManager.cpp$(DependSuffix) -MM "BatchManager.cpp"

$(IntermediateDirectory)/BatchManager.cpp$(PreprocessSuffix): BatchManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BatchManager.cpp$(PreprocessSuffix) "BatchManager.cpp"

$(IntermediateDirectory)/Camera2D.cpp$(ObjectSuffix): Camera2D.cpp $(IntermediateDirectory)/Camera2D.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Camera2D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Camera2D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Camera2D.cpp$(DependSuffix): Camera2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Camera2D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Camera2D.cpp$(DependSuffix) -MM "Camera2D.cpp"

$(IntermediateDirectory)/Camera2D.cpp$(PreprocessSuffix): Camera2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Camera2D.cpp$(PreprocessSuffix) "Camera2D.cpp"

$(IntermediateDirectory)/Collider.cpp$(ObjectSuffix): Collider.cpp $(IntermediateDirectory)/Collider.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Collider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Collider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Collider.cpp$(DependSuffix): Collider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Collider.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Collider.cpp$(DependSuffix) -MM "Collider.cpp"

$(IntermediateDirectory)/Collider.cpp$(PreprocessSuffix): Collider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Collider.cpp$(PreprocessSuffix) "Collider.cpp"

$(IntermediateDirectory)/Component.cpp$(ObjectSuffix): Component.cpp $(IntermediateDirectory)/Component.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Component.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Component.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Component.cpp$(DependSuffix): Component.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Component.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Component.cpp$(DependSuffix) -MM "Component.cpp"

$(IntermediateDirectory)/Component.cpp$(PreprocessSuffix): Component.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Component.cpp$(PreprocessSuffix) "Component.cpp"

$(IntermediateDirectory)/Compression.cpp$(ObjectSuffix): Compression.cpp $(IntermediateDirectory)/Compression.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Compression.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Compression.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Compression.cpp$(DependSuffix): Compression.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Compression.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Compression.cpp$(DependSuffix) -MM "Compression.cpp"

$(IntermediateDirectory)/Compression.cpp$(PreprocessSuffix): Compression.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Compression.cpp$(PreprocessSuffix) "Compression.cpp"

$(IntermediateDirectory)/Console.cpp$(ObjectSuffix): Console.cpp $(IntermediateDirectory)/Console.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Console.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Console.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Console.cpp$(DependSuffix): Console.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Console.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Console.cpp$(DependSuffix) -MM "Console.cpp"

$(IntermediateDirectory)/Console.cpp$(PreprocessSuffix): Console.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Console.cpp$(PreprocessSuffix) "Console.cpp"

$(IntermediateDirectory)/Exceptions.cpp$(ObjectSuffix): Exceptions.cpp $(IntermediateDirectory)/Exceptions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Exceptions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Exceptions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Exceptions.cpp$(DependSuffix): Exceptions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Exceptions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Exceptions.cpp$(DependSuffix) -MM "Exceptions.cpp"

$(IntermediateDirectory)/Exceptions.cpp$(PreprocessSuffix): Exceptions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Exceptions.cpp$(PreprocessSuffix) "Exceptions.cpp"

$(IntermediateDirectory)/FileStream.cpp$(ObjectSuffix): FileStream.cpp $(IntermediateDirectory)/FileStream.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/FileStream.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FileStream.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FileStream.cpp$(DependSuffix): FileStream.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FileStream.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FileStream.cpp$(DependSuffix) -MM "FileStream.cpp"

$(IntermediateDirectory)/FileStream.cpp$(PreprocessSuffix): FileStream.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FileStream.cpp$(PreprocessSuffix) "FileStream.cpp"

$(IntermediateDirectory)/GLSLProgram.cpp$(ObjectSuffix): GLSLProgram.cpp $(IntermediateDirectory)/GLSLProgram.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GLSLProgram.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GLSLProgram.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GLSLProgram.cpp$(DependSuffix): GLSLProgram.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GLSLProgram.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GLSLProgram.cpp$(DependSuffix) -MM "GLSLProgram.cpp"

$(IntermediateDirectory)/GLSLProgram.cpp$(PreprocessSuffix): GLSLProgram.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GLSLProgram.cpp$(PreprocessSuffix) "GLSLProgram.cpp"

$(IntermediateDirectory)/GUICheckbox.cpp$(ObjectSuffix): GUICheckbox.cpp $(IntermediateDirectory)/GUICheckbox.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUICheckbox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUICheckbox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUICheckbox.cpp$(DependSuffix): GUICheckbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUICheckbox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUICheckbox.cpp$(DependSuffix) -MM "GUICheckbox.cpp"

$(IntermediateDirectory)/GUICheckbox.cpp$(PreprocessSuffix): GUICheckbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUICheckbox.cpp$(PreprocessSuffix) "GUICheckbox.cpp"

$(IntermediateDirectory)/GUIColorEditor.cpp$(ObjectSuffix): GUIColorEditor.cpp $(IntermediateDirectory)/GUIColorEditor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIColorEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIColorEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIColorEditor.cpp$(DependSuffix): GUIColorEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIColorEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIColorEditor.cpp$(DependSuffix) -MM "GUIColorEditor.cpp"

$(IntermediateDirectory)/GUIColorEditor.cpp$(PreprocessSuffix): GUIColorEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIColorEditor.cpp$(PreprocessSuffix) "GUIColorEditor.cpp"

$(IntermediateDirectory)/GUID.cpp$(ObjectSuffix): GUID.cpp $(IntermediateDirectory)/GUID.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUID.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUID.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUID.cpp$(DependSuffix): GUID.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUID.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUID.cpp$(DependSuffix) -MM "GUID.cpp"

$(IntermediateDirectory)/GUID.cpp$(PreprocessSuffix): GUID.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUID.cpp$(PreprocessSuffix) "GUID.cpp"

$(IntermediateDirectory)/GUIPopup.cpp$(ObjectSuffix): GUIPopup.cpp $(IntermediateDirectory)/GUIPopup.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIPopup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIPopup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIPopup.cpp$(DependSuffix): GUIPopup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIPopup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIPopup.cpp$(DependSuffix) -MM "GUIPopup.cpp"

$(IntermediateDirectory)/GUIPopup.cpp$(PreprocessSuffix): GUIPopup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIPopup.cpp$(PreprocessSuffix) "GUIPopup.cpp"

$(IntermediateDirectory)/GUIRectangle.cpp$(ObjectSuffix): GUIRectangle.cpp $(IntermediateDirectory)/GUIRectangle.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangle.cpp$(DependSuffix): GUIRectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangle.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangle.cpp$(DependSuffix) -MM "GUIRectangle.cpp"

$(IntermediateDirectory)/GUIRectangle.cpp$(PreprocessSuffix): GUIRectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangle.cpp$(PreprocessSuffix) "GUIRectangle.cpp"

$(IntermediateDirectory)/GUIRectangleColumn.cpp$(ObjectSuffix): GUIRectangleColumn.cpp $(IntermediateDirectory)/GUIRectangleColumn.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleColumn.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleColumn.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleColumn.cpp$(DependSuffix): GUIRectangleColumn.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleColumn.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleColumn.cpp$(DependSuffix) -MM "GUIRectangleColumn.cpp"

$(IntermediateDirectory)/GUIRectangleColumn.cpp$(PreprocessSuffix): GUIRectangleColumn.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleColumn.cpp$(PreprocessSuffix) "GUIRectangleColumn.cpp"

$(IntermediateDirectory)/GUIRectangleContainer.cpp$(ObjectSuffix): GUIRectangleContainer.cpp $(IntermediateDirectory)/GUIRectangleContainer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleContainer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleContainer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleContainer.cpp$(DependSuffix): GUIRectangleContainer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleContainer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleContainer.cpp$(DependSuffix) -MM "GUIRectangleContainer.cpp"

$(IntermediateDirectory)/GUIRectangleContainer.cpp$(PreprocessSuffix): GUIRectangleContainer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleContainer.cpp$(PreprocessSuffix) "GUIRectangleContainer.cpp"

$(IntermediateDirectory)/GUIRectangleRow.cpp$(ObjectSuffix): GUIRectangleRow.cpp $(IntermediateDirectory)/GUIRectangleRow.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleRow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleRow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleRow.cpp$(DependSuffix): GUIRectangleRow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleRow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleRow.cpp$(DependSuffix) -MM "GUIRectangleRow.cpp"

$(IntermediateDirectory)/GUIRectangleRow.cpp$(PreprocessSuffix): GUIRectangleRow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleRow.cpp$(PreprocessSuffix) "GUIRectangleRow.cpp"

$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(ObjectSuffix): GUIRectangleScrollList.cpp $(IntermediateDirectory)/GUIRectangleScrollList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleScrollList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(DependSuffix): GUIRectangleScrollList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(DependSuffix) -MM "GUIRectangleScrollList.cpp"

$(IntermediateDirectory)/GUIRectangleScrollList.cpp$(PreprocessSuffix): GUIRectangleScrollList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleScrollList.cpp$(PreprocessSuffix) "GUIRectangleScrollList.cpp"

$(IntermediateDirectory)/GUIRectangleTable.cpp$(ObjectSuffix): GUIRectangleTable.cpp $(IntermediateDirectory)/GUIRectangleTable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleTable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleTable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleTable.cpp$(DependSuffix): GUIRectangleTable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleTable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleTable.cpp$(DependSuffix) -MM "GUIRectangleTable.cpp"

$(IntermediateDirectory)/GUIRectangleTable.cpp$(PreprocessSuffix): GUIRectangleTable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleTable.cpp$(PreprocessSuffix) "GUIRectangleTable.cpp"

$(IntermediateDirectory)/GUIRectangleTree.cpp$(ObjectSuffix): GUIRectangleTree.cpp $(IntermediateDirectory)/GUIRectangleTree.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIRectangleTree.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIRectangleTree.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIRectangleTree.cpp$(DependSuffix): GUIRectangleTree.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIRectangleTree.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIRectangleTree.cpp$(DependSuffix) -MM "GUIRectangleTree.cpp"

$(IntermediateDirectory)/GUIRectangleTree.cpp$(PreprocessSuffix): GUIRectangleTree.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIRectangleTree.cpp$(PreprocessSuffix) "GUIRectangleTree.cpp"

$(IntermediateDirectory)/GUIStringEditor.cpp$(ObjectSuffix): GUIStringEditor.cpp $(IntermediateDirectory)/GUIStringEditor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIStringEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIStringEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIStringEditor.cpp$(DependSuffix): GUIStringEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIStringEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIStringEditor.cpp$(DependSuffix) -MM "GUIStringEditor.cpp"

$(IntermediateDirectory)/GUIStringEditor.cpp$(PreprocessSuffix): GUIStringEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIStringEditor.cpp$(PreprocessSuffix) "GUIStringEditor.cpp"

$(IntermediateDirectory)/GUIWindow.cpp$(ObjectSuffix): GUIWindow.cpp $(IntermediateDirectory)/GUIWindow.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIWindow.cpp$(DependSuffix): GUIWindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIWindow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIWindow.cpp$(DependSuffix) -MM "GUIWindow.cpp"

$(IntermediateDirectory)/GUIWindow.cpp$(PreprocessSuffix): GUIWindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIWindow.cpp$(PreprocessSuffix) "GUIWindow.cpp"

$(IntermediateDirectory)/GUIWindowManager.cpp$(ObjectSuffix): GUIWindowManager.cpp $(IntermediateDirectory)/GUIWindowManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GUIWindowManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GUIWindowManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GUIWindowManager.cpp$(DependSuffix): GUIWindowManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GUIWindowManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GUIWindowManager.cpp$(DependSuffix) -MM "GUIWindowManager.cpp"

$(IntermediateDirectory)/GUIWindowManager.cpp$(PreprocessSuffix): GUIWindowManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GUIWindowManager.cpp$(PreprocessSuffix) "GUIWindowManager.cpp"

$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix): GameObject.cpp $(IntermediateDirectory)/GameObject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/GameObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GameObject.cpp$(DependSuffix): GameObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GameObject.cpp$(DependSuffix) -MM "GameObject.cpp"

$(IntermediateDirectory)/GameObject.cpp$(PreprocessSuffix): GameObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GameObject.cpp$(PreprocessSuffix) "GameObject.cpp"

$(IntermediateDirectory)/Geometry.cpp$(ObjectSuffix): Geometry.cpp $(IntermediateDirectory)/Geometry.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Geometry.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Geometry.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Geometry.cpp$(DependSuffix): Geometry.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Geometry.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Geometry.cpp$(DependSuffix) -MM "Geometry.cpp"

$(IntermediateDirectory)/Geometry.cpp$(PreprocessSuffix): Geometry.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Geometry.cpp$(PreprocessSuffix) "Geometry.cpp"

$(IntermediateDirectory)/InputManager.cpp$(ObjectSuffix): InputManager.cpp $(IntermediateDirectory)/InputManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/InputManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/InputManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/InputManager.cpp$(DependSuffix): InputManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/InputManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/InputManager.cpp$(DependSuffix) -MM "InputManager.cpp"

$(IntermediateDirectory)/InputManager.cpp$(PreprocessSuffix): InputManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/InputManager.cpp$(PreprocessSuffix) "InputManager.cpp"

$(IntermediateDirectory)/KeyboardRecorder.cpp$(ObjectSuffix): KeyboardRecorder.cpp $(IntermediateDirectory)/KeyboardRecorder.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/KeyboardRecorder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/KeyboardRecorder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/KeyboardRecorder.cpp$(DependSuffix): KeyboardRecorder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/KeyboardRecorder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/KeyboardRecorder.cpp$(DependSuffix) -MM "KeyboardRecorder.cpp"

$(IntermediateDirectory)/KeyboardRecorder.cpp$(PreprocessSuffix): KeyboardRecorder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/KeyboardRecorder.cpp$(PreprocessSuffix) "KeyboardRecorder.cpp"

$(IntermediateDirectory)/LengthConversion.cpp$(ObjectSuffix): LengthConversion.cpp $(IntermediateDirectory)/LengthConversion.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/LengthConversion.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LengthConversion.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LengthConversion.cpp$(DependSuffix): LengthConversion.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LengthConversion.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LengthConversion.cpp$(DependSuffix) -MM "LengthConversion.cpp"

$(IntermediateDirectory)/LengthConversion.cpp$(PreprocessSuffix): LengthConversion.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LengthConversion.cpp$(PreprocessSuffix) "LengthConversion.cpp"

$(IntermediateDirectory)/Line.cpp$(ObjectSuffix): Line.cpp $(IntermediateDirectory)/Line.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Line.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Line.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Line.cpp$(DependSuffix): Line.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Line.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Line.cpp$(DependSuffix) -MM "Line.cpp"

$(IntermediateDirectory)/Line.cpp$(PreprocessSuffix): Line.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Line.cpp$(PreprocessSuffix) "Line.cpp"

$(IntermediateDirectory)/OS.cpp$(ObjectSuffix): OS.cpp $(IntermediateDirectory)/OS.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/OS.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OS.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OS.cpp$(DependSuffix): OS.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OS.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OS.cpp$(DependSuffix) -MM "OS.cpp"

$(IntermediateDirectory)/OS.cpp$(PreprocessSuffix): OS.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OS.cpp$(PreprocessSuffix) "OS.cpp"

$(IntermediateDirectory)/OpenALError.cpp$(ObjectSuffix): OpenALError.cpp $(IntermediateDirectory)/OpenALError.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/OpenALError.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OpenALError.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OpenALError.cpp$(DependSuffix): OpenALError.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OpenALError.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OpenALError.cpp$(DependSuffix) -MM "OpenALError.cpp"

$(IntermediateDirectory)/OpenALError.cpp$(PreprocessSuffix): OpenALError.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OpenALError.cpp$(PreprocessSuffix) "OpenALError.cpp"

$(IntermediateDirectory)/OpenGLError.cpp$(ObjectSuffix): OpenGLError.cpp $(IntermediateDirectory)/OpenGLError.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/OpenGLError.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OpenGLError.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OpenGLError.cpp$(DependSuffix): OpenGLError.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OpenGLError.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OpenGLError.cpp$(DependSuffix) -MM "OpenGLError.cpp"

$(IntermediateDirectory)/OpenGLError.cpp$(PreprocessSuffix): OpenGLError.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OpenGLError.cpp$(PreprocessSuffix) "OpenGLError.cpp"

$(IntermediateDirectory)/PhysicsWorld2D.cpp$(ObjectSuffix): PhysicsWorld2D.cpp $(IntermediateDirectory)/PhysicsWorld2D.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/PhysicsWorld2D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsWorld2D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsWorld2D.cpp$(DependSuffix): PhysicsWorld2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PhysicsWorld2D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PhysicsWorld2D.cpp$(DependSuffix) -MM "PhysicsWorld2D.cpp"

$(IntermediateDirectory)/PhysicsWorld2D.cpp$(PreprocessSuffix): PhysicsWorld2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PhysicsWorld2D.cpp$(PreprocessSuffix) "PhysicsWorld2D.cpp"

$(IntermediateDirectory)/Point.cpp$(ObjectSuffix): Point.cpp $(IntermediateDirectory)/Point.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Point.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Point.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Point.cpp$(DependSuffix): Point.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Point.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Point.cpp$(DependSuffix) -MM "Point.cpp"

$(IntermediateDirectory)/Point.cpp$(PreprocessSuffix): Point.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Point.cpp$(PreprocessSuffix) "Point.cpp"

$(IntermediateDirectory)/Polygon.cpp$(ObjectSuffix): Polygon.cpp $(IntermediateDirectory)/Polygon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Polygon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Polygon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Polygon.cpp$(DependSuffix): Polygon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Polygon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Polygon.cpp$(DependSuffix) -MM "Polygon.cpp"

$(IntermediateDirectory)/Polygon.cpp$(PreprocessSuffix): Polygon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Polygon.cpp$(PreprocessSuffix) "Polygon.cpp"

$(IntermediateDirectory)/Position64.cpp$(ObjectSuffix): Position64.cpp $(IntermediateDirectory)/Position64.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Position64.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Position64.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Position64.cpp$(DependSuffix): Position64.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Position64.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Position64.cpp$(DependSuffix) -MM "Position64.cpp"

$(IntermediateDirectory)/Position64.cpp$(PreprocessSuffix): Position64.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Position64.cpp$(PreprocessSuffix) "Position64.cpp"

$(IntermediateDirectory)/PostProcessing.cpp$(ObjectSuffix): PostProcessing.cpp $(IntermediateDirectory)/PostProcessing.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/PostProcessing.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PostProcessing.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PostProcessing.cpp$(DependSuffix): PostProcessing.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PostProcessing.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PostProcessing.cpp$(DependSuffix) -MM "PostProcessing.cpp"

$(IntermediateDirectory)/PostProcessing.cpp$(PreprocessSuffix): PostProcessing.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PostProcessing.cpp$(PreprocessSuffix) "PostProcessing.cpp"

$(IntermediateDirectory)/Primitive.cpp$(ObjectSuffix): Primitive.cpp $(IntermediateDirectory)/Primitive.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Primitive.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Primitive.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Primitive.cpp$(DependSuffix): Primitive.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Primitive.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Primitive.cpp$(DependSuffix) -MM "Primitive.cpp"

$(IntermediateDirectory)/Primitive.cpp$(PreprocessSuffix): Primitive.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Primitive.cpp$(PreprocessSuffix) "Primitive.cpp"

$(IntermediateDirectory)/RNG.cpp$(ObjectSuffix): RNG.cpp $(IntermediateDirectory)/RNG.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/RNG.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RNG.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RNG.cpp$(DependSuffix): RNG.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/RNG.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/RNG.cpp$(DependSuffix) -MM "RNG.cpp"

$(IntermediateDirectory)/RNG.cpp$(PreprocessSuffix): RNG.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/RNG.cpp$(PreprocessSuffix) "RNG.cpp"

$(IntermediateDirectory)/RigidBody2D.cpp$(ObjectSuffix): RigidBody2D.cpp $(IntermediateDirectory)/RigidBody2D.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/RigidBody2D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RigidBody2D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RigidBody2D.cpp$(DependSuffix): RigidBody2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/RigidBody2D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/RigidBody2D.cpp$(DependSuffix) -MM "RigidBody2D.cpp"

$(IntermediateDirectory)/RigidBody2D.cpp$(PreprocessSuffix): RigidBody2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/RigidBody2D.cpp$(PreprocessSuffix) "RigidBody2D.cpp"

$(IntermediateDirectory)/SATCollision.cpp$(ObjectSuffix): SATCollision.cpp $(IntermediateDirectory)/SATCollision.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/SATCollision.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SATCollision.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SATCollision.cpp$(DependSuffix): SATCollision.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SATCollision.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SATCollision.cpp$(DependSuffix) -MM "SATCollision.cpp"

$(IntermediateDirectory)/SATCollision.cpp$(PreprocessSuffix): SATCollision.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SATCollision.cpp$(PreprocessSuffix) "SATCollision.cpp"

$(IntermediateDirectory)/Serializable.cpp$(ObjectSuffix): Serializable.cpp $(IntermediateDirectory)/Serializable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Serializable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Serializable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Serializable.cpp$(DependSuffix): Serializable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Serializable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Serializable.cpp$(DependSuffix) -MM "Serializable.cpp"

$(IntermediateDirectory)/Serializable.cpp$(PreprocessSuffix): Serializable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Serializable.cpp$(PreprocessSuffix) "Serializable.cpp"

$(IntermediateDirectory)/SerializableDirectory.cpp$(ObjectSuffix): SerializableDirectory.cpp $(IntermediateDirectory)/SerializableDirectory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/SerializableDirectory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerializableDirectory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerializableDirectory.cpp$(DependSuffix): SerializableDirectory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerializableDirectory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerializableDirectory.cpp$(DependSuffix) -MM "SerializableDirectory.cpp"

$(IntermediateDirectory)/SerializableDirectory.cpp$(PreprocessSuffix): SerializableDirectory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerializableDirectory.cpp$(PreprocessSuffix) "SerializableDirectory.cpp"

$(IntermediateDirectory)/SerializableString.cpp$(ObjectSuffix): SerializableString.cpp $(IntermediateDirectory)/SerializableString.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/SerializableString.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerializableString.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerializableString.cpp$(DependSuffix): SerializableString.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerializableString.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerializableString.cpp$(DependSuffix) -MM "SerializableString.cpp"

$(IntermediateDirectory)/SerializableString.cpp$(PreprocessSuffix): SerializableString.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerializableString.cpp$(PreprocessSuffix) "SerializableString.cpp"

$(IntermediateDirectory)/ShaderManager.cpp$(ObjectSuffix): ShaderManager.cpp $(IntermediateDirectory)/ShaderManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/ShaderManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ShaderManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ShaderManager.cpp$(DependSuffix): ShaderManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ShaderManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ShaderManager.cpp$(DependSuffix) -MM "ShaderManager.cpp"

$(IntermediateDirectory)/ShaderManager.cpp$(PreprocessSuffix): ShaderManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ShaderManager.cpp$(PreprocessSuffix) "ShaderManager.cpp"

$(IntermediateDirectory)/Shapes.cpp$(ObjectSuffix): Shapes.cpp $(IntermediateDirectory)/Shapes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Shapes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Shapes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Shapes.cpp$(DependSuffix): Shapes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Shapes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Shapes.cpp$(DependSuffix) -MM "Shapes.cpp"

$(IntermediateDirectory)/Shapes.cpp$(PreprocessSuffix): Shapes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Shapes.cpp$(PreprocessSuffix) "Shapes.cpp"

$(IntermediateDirectory)/SoundSource.cpp$(ObjectSuffix): SoundSource.cpp $(IntermediateDirectory)/SoundSource.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/SoundSource.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SoundSource.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SoundSource.cpp$(DependSuffix): SoundSource.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SoundSource.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SoundSource.cpp$(DependSuffix) -MM "SoundSource.cpp"

$(IntermediateDirectory)/SoundSource.cpp$(PreprocessSuffix): SoundSource.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SoundSource.cpp$(PreprocessSuffix) "SoundSource.cpp"

$(IntermediateDirectory)/SpehsEngine.cpp$(ObjectSuffix): SpehsEngine.cpp $(IntermediateDirectory)/SpehsEngine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/SpehsEngine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SpehsEngine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SpehsEngine.cpp$(DependSuffix): SpehsEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SpehsEngine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SpehsEngine.cpp$(DependSuffix) -MM "SpehsEngine.cpp"

$(IntermediateDirectory)/SpehsEngine.cpp$(PreprocessSuffix): SpehsEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SpehsEngine.cpp$(PreprocessSuffix) "SpehsEngine.cpp"

$(IntermediateDirectory)/Sprite.cpp$(ObjectSuffix): Sprite.cpp $(IntermediateDirectory)/Sprite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Sprite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Sprite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Sprite.cpp$(DependSuffix): Sprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Sprite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Sprite.cpp$(DependSuffix) -MM "Sprite.cpp"

$(IntermediateDirectory)/Sprite.cpp$(PreprocessSuffix): Sprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Sprite.cpp$(PreprocessSuffix) "Sprite.cpp"

$(IntermediateDirectory)/StringOperations.cpp$(ObjectSuffix): StringOperations.cpp $(IntermediateDirectory)/StringOperations.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/StringOperations.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StringOperations.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StringOperations.cpp$(DependSuffix): StringOperations.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StringOperations.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StringOperations.cpp$(DependSuffix) -MM "StringOperations.cpp"

$(IntermediateDirectory)/StringOperations.cpp$(PreprocessSuffix): StringOperations.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StringOperations.cpp$(PreprocessSuffix) "StringOperations.cpp"

$(IntermediateDirectory)/Text.cpp$(ObjectSuffix): Text.cpp $(IntermediateDirectory)/Text.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Text.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Text.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Text.cpp$(DependSuffix): Text.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Text.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Text.cpp$(DependSuffix) -MM "Text.cpp"

$(IntermediateDirectory)/Text.cpp$(PreprocessSuffix): Text.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Text.cpp$(PreprocessSuffix) "Text.cpp"

$(IntermediateDirectory)/Textbox.cpp$(ObjectSuffix): Textbox.cpp $(IntermediateDirectory)/Textbox.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Textbox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Textbox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Textbox.cpp$(DependSuffix): Textbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Textbox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Textbox.cpp$(DependSuffix) -MM "Textbox.cpp"

$(IntermediateDirectory)/Textbox.cpp$(PreprocessSuffix): Textbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Textbox.cpp$(PreprocessSuffix) "Textbox.cpp"

$(IntermediateDirectory)/TextureManager.cpp$(ObjectSuffix): TextureManager.cpp $(IntermediateDirectory)/TextureManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/TextureManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TextureManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TextureManager.cpp$(DependSuffix): TextureManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TextureManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/TextureManager.cpp$(DependSuffix) -MM "TextureManager.cpp"

$(IntermediateDirectory)/TextureManager.cpp$(PreprocessSuffix): TextureManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TextureManager.cpp$(PreprocessSuffix) "TextureManager.cpp"

$(IntermediateDirectory)/Thread.cpp$(ObjectSuffix): Thread.cpp $(IntermediateDirectory)/Thread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Thread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Thread.cpp$(DependSuffix): Thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Thread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Thread.cpp$(DependSuffix) -MM "Thread.cpp"

$(IntermediateDirectory)/Thread.cpp$(PreprocessSuffix): Thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Thread.cpp$(PreprocessSuffix) "Thread.cpp"

$(IntermediateDirectory)/Time.cpp$(ObjectSuffix): Time.cpp $(IntermediateDirectory)/Time.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Time.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Time.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Time.cpp$(DependSuffix): Time.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Time.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Time.cpp$(DependSuffix) -MM "Time.cpp"

$(IntermediateDirectory)/Time.cpp$(PreprocessSuffix): Time.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Time.cpp$(PreprocessSuffix) "Time.cpp"

$(IntermediateDirectory)/Transform2D.cpp$(ObjectSuffix): Transform2D.cpp $(IntermediateDirectory)/Transform2D.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Transform2D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Transform2D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Transform2D.cpp$(DependSuffix): Transform2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Transform2D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Transform2D.cpp$(DependSuffix) -MM "Transform2D.cpp"

$(IntermediateDirectory)/Transform2D.cpp$(PreprocessSuffix): Transform2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Transform2D.cpp$(PreprocessSuffix) "Transform2D.cpp"

$(IntermediateDirectory)/Triangulation.cpp$(ObjectSuffix): Triangulation.cpp $(IntermediateDirectory)/Triangulation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Triangulation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Triangulation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Triangulation.cpp$(DependSuffix): Triangulation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Triangulation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Triangulation.cpp$(DependSuffix) -MM "Triangulation.cpp"

$(IntermediateDirectory)/Triangulation.cpp$(PreprocessSuffix): Triangulation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Triangulation.cpp$(PreprocessSuffix) "Triangulation.cpp"

$(IntermediateDirectory)/Vertex.cpp$(ObjectSuffix): Vertex.cpp $(IntermediateDirectory)/Vertex.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Vertex.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Vertex.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Vertex.cpp$(DependSuffix): Vertex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Vertex.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Vertex.cpp$(DependSuffix) -MM "Vertex.cpp"

$(IntermediateDirectory)/Vertex.cpp$(PreprocessSuffix): Vertex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Vertex.cpp$(PreprocessSuffix) "Vertex.cpp"

$(IntermediateDirectory)/Window.cpp$(ObjectSuffix): Window.cpp $(IntermediateDirectory)/Window.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/projects/SpehsEngine/SpehsEngine/Window.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Window.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Window.cpp$(DependSuffix): Window.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Window.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Window.cpp$(DependSuffix) -MM "Window.cpp"

$(IntermediateDirectory)/Window.cpp$(PreprocessSuffix): Window.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Window.cpp$(PreprocessSuffix) "Window.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) "../../SpehsRebuut/.build-debug/SpehsEngine"


