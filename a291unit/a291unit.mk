##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=a291unit
ConfigurationName      :=Debug
WorkspacePath          :=/home/pi/cpp_projects/a291
ProjectPath            :=/home/pi/cpp_projects/a291/a291unit
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=root
Date                   :=05/19/21
CodeLitePath           :=/root/.codelite
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
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=$(PreprocessorSwitch)DEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="a291unit.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
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
CXXFLAGS :=  -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/wireBusControler.cpp$(ObjectSuffix) $(IntermediateDirectory)/configurationList.cpp$(ObjectSuffix) $(IntermediateDirectory)/mqttBroker.cpp$(ObjectSuffix) $(IntermediateDirectory)/bme280.c$(ObjectSuffix) $(IntermediateDirectory)/ds18b20SensorGroup.cpp$(ObjectSuffix) $(IntermediateDirectory)/dhtxxSensor.cpp$(ObjectSuffix) $(IntermediateDirectory)/gpioInput.cpp$(ObjectSuffix) $(IntermediateDirectory)/mqttOutput.cpp$(ObjectSuffix) $(IntermediateDirectory)/pinIOControler.cpp$(ObjectSuffix) $(IntermediateDirectory)/configurabel.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/gpioManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/dhtControler.cpp$(ObjectSuffix) $(IntermediateDirectory)/registrationNumber.cpp$(ObjectSuffix) $(IntermediateDirectory)/unitManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/bme280Sensor.cpp$(ObjectSuffix) $(IntermediateDirectory)/ds18b20Sensor.cpp$(ObjectSuffix) $(IntermediateDirectory)/bme280Controler.cpp$(ObjectSuffix) $(IntermediateDirectory)/mqttClient.cpp$(ObjectSuffix) $(IntermediateDirectory)/controler.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "/home/pi/cpp_projects/a291/.build-debug"
	@echo rebuilt > "/home/pi/cpp_projects/a291/.build-debug/a291unit"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/wireBusControler.cpp$(ObjectSuffix): wireBusControler.cpp $(IntermediateDirectory)/wireBusControler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/wireBusControler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wireBusControler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wireBusControler.cpp$(DependSuffix): wireBusControler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wireBusControler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wireBusControler.cpp$(DependSuffix) -MM wireBusControler.cpp

$(IntermediateDirectory)/wireBusControler.cpp$(PreprocessSuffix): wireBusControler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wireBusControler.cpp$(PreprocessSuffix) wireBusControler.cpp

$(IntermediateDirectory)/configurationList.cpp$(ObjectSuffix): configurationList.cpp $(IntermediateDirectory)/configurationList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/configurationList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/configurationList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/configurationList.cpp$(DependSuffix): configurationList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/configurationList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/configurationList.cpp$(DependSuffix) -MM configurationList.cpp

$(IntermediateDirectory)/configurationList.cpp$(PreprocessSuffix): configurationList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/configurationList.cpp$(PreprocessSuffix) configurationList.cpp

$(IntermediateDirectory)/mqttBroker.cpp$(ObjectSuffix): mqttBroker.cpp $(IntermediateDirectory)/mqttBroker.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/mqttBroker.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mqttBroker.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mqttBroker.cpp$(DependSuffix): mqttBroker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mqttBroker.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mqttBroker.cpp$(DependSuffix) -MM mqttBroker.cpp

$(IntermediateDirectory)/mqttBroker.cpp$(PreprocessSuffix): mqttBroker.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mqttBroker.cpp$(PreprocessSuffix) mqttBroker.cpp

$(IntermediateDirectory)/bme280.c$(ObjectSuffix): bme280.c $(IntermediateDirectory)/bme280.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/bme280.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bme280.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bme280.c$(DependSuffix): bme280.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bme280.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bme280.c$(DependSuffix) -MM bme280.c

$(IntermediateDirectory)/bme280.c$(PreprocessSuffix): bme280.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bme280.c$(PreprocessSuffix) bme280.c

$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(ObjectSuffix): ds18b20SensorGroup.cpp $(IntermediateDirectory)/ds18b20SensorGroup.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/ds18b20SensorGroup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(DependSuffix): ds18b20SensorGroup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(DependSuffix) -MM ds18b20SensorGroup.cpp

$(IntermediateDirectory)/ds18b20SensorGroup.cpp$(PreprocessSuffix): ds18b20SensorGroup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ds18b20SensorGroup.cpp$(PreprocessSuffix) ds18b20SensorGroup.cpp

$(IntermediateDirectory)/dhtxxSensor.cpp$(ObjectSuffix): dhtxxSensor.cpp $(IntermediateDirectory)/dhtxxSensor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/dhtxxSensor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dhtxxSensor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dhtxxSensor.cpp$(DependSuffix): dhtxxSensor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dhtxxSensor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dhtxxSensor.cpp$(DependSuffix) -MM dhtxxSensor.cpp

$(IntermediateDirectory)/dhtxxSensor.cpp$(PreprocessSuffix): dhtxxSensor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dhtxxSensor.cpp$(PreprocessSuffix) dhtxxSensor.cpp

$(IntermediateDirectory)/gpioInput.cpp$(ObjectSuffix): gpioInput.cpp $(IntermediateDirectory)/gpioInput.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/gpioInput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/gpioInput.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/gpioInput.cpp$(DependSuffix): gpioInput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/gpioInput.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/gpioInput.cpp$(DependSuffix) -MM gpioInput.cpp

$(IntermediateDirectory)/gpioInput.cpp$(PreprocessSuffix): gpioInput.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/gpioInput.cpp$(PreprocessSuffix) gpioInput.cpp

$(IntermediateDirectory)/mqttOutput.cpp$(ObjectSuffix): mqttOutput.cpp $(IntermediateDirectory)/mqttOutput.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/mqttOutput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mqttOutput.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mqttOutput.cpp$(DependSuffix): mqttOutput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mqttOutput.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mqttOutput.cpp$(DependSuffix) -MM mqttOutput.cpp

$(IntermediateDirectory)/mqttOutput.cpp$(PreprocessSuffix): mqttOutput.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mqttOutput.cpp$(PreprocessSuffix) mqttOutput.cpp

$(IntermediateDirectory)/pinIOControler.cpp$(ObjectSuffix): pinIOControler.cpp $(IntermediateDirectory)/pinIOControler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/pinIOControler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/pinIOControler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/pinIOControler.cpp$(DependSuffix): pinIOControler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/pinIOControler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/pinIOControler.cpp$(DependSuffix) -MM pinIOControler.cpp

$(IntermediateDirectory)/pinIOControler.cpp$(PreprocessSuffix): pinIOControler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/pinIOControler.cpp$(PreprocessSuffix) pinIOControler.cpp

$(IntermediateDirectory)/configurabel.cpp$(ObjectSuffix): configurabel.cpp $(IntermediateDirectory)/configurabel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/configurabel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/configurabel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/configurabel.cpp$(DependSuffix): configurabel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/configurabel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/configurabel.cpp$(DependSuffix) -MM configurabel.cpp

$(IntermediateDirectory)/configurabel.cpp$(PreprocessSuffix): configurabel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/configurabel.cpp$(PreprocessSuffix) configurabel.cpp

$(IntermediateDirectory)/gpioManager.cpp$(ObjectSuffix): gpioManager.cpp $(IntermediateDirectory)/gpioManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/gpioManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/gpioManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/gpioManager.cpp$(DependSuffix): gpioManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/gpioManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/gpioManager.cpp$(DependSuffix) -MM gpioManager.cpp

$(IntermediateDirectory)/gpioManager.cpp$(PreprocessSuffix): gpioManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/gpioManager.cpp$(PreprocessSuffix) gpioManager.cpp

$(IntermediateDirectory)/dhtControler.cpp$(ObjectSuffix): dhtControler.cpp $(IntermediateDirectory)/dhtControler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/dhtControler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dhtControler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dhtControler.cpp$(DependSuffix): dhtControler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dhtControler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dhtControler.cpp$(DependSuffix) -MM dhtControler.cpp

$(IntermediateDirectory)/dhtControler.cpp$(PreprocessSuffix): dhtControler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dhtControler.cpp$(PreprocessSuffix) dhtControler.cpp

$(IntermediateDirectory)/registrationNumber.cpp$(ObjectSuffix): registrationNumber.cpp $(IntermediateDirectory)/registrationNumber.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/registrationNumber.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/registrationNumber.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/registrationNumber.cpp$(DependSuffix): registrationNumber.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/registrationNumber.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/registrationNumber.cpp$(DependSuffix) -MM registrationNumber.cpp

$(IntermediateDirectory)/registrationNumber.cpp$(PreprocessSuffix): registrationNumber.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/registrationNumber.cpp$(PreprocessSuffix) registrationNumber.cpp

$(IntermediateDirectory)/unitManager.cpp$(ObjectSuffix): unitManager.cpp $(IntermediateDirectory)/unitManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/unitManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/unitManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/unitManager.cpp$(DependSuffix): unitManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/unitManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/unitManager.cpp$(DependSuffix) -MM unitManager.cpp

$(IntermediateDirectory)/unitManager.cpp$(PreprocessSuffix): unitManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/unitManager.cpp$(PreprocessSuffix) unitManager.cpp

$(IntermediateDirectory)/bme280Sensor.cpp$(ObjectSuffix): bme280Sensor.cpp $(IntermediateDirectory)/bme280Sensor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/bme280Sensor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bme280Sensor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bme280Sensor.cpp$(DependSuffix): bme280Sensor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bme280Sensor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bme280Sensor.cpp$(DependSuffix) -MM bme280Sensor.cpp

$(IntermediateDirectory)/bme280Sensor.cpp$(PreprocessSuffix): bme280Sensor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bme280Sensor.cpp$(PreprocessSuffix) bme280Sensor.cpp

$(IntermediateDirectory)/ds18b20Sensor.cpp$(ObjectSuffix): ds18b20Sensor.cpp $(IntermediateDirectory)/ds18b20Sensor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/ds18b20Sensor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ds18b20Sensor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ds18b20Sensor.cpp$(DependSuffix): ds18b20Sensor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ds18b20Sensor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ds18b20Sensor.cpp$(DependSuffix) -MM ds18b20Sensor.cpp

$(IntermediateDirectory)/ds18b20Sensor.cpp$(PreprocessSuffix): ds18b20Sensor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ds18b20Sensor.cpp$(PreprocessSuffix) ds18b20Sensor.cpp

$(IntermediateDirectory)/bme280Controler.cpp$(ObjectSuffix): bme280Controler.cpp $(IntermediateDirectory)/bme280Controler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/bme280Controler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bme280Controler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bme280Controler.cpp$(DependSuffix): bme280Controler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bme280Controler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bme280Controler.cpp$(DependSuffix) -MM bme280Controler.cpp

$(IntermediateDirectory)/bme280Controler.cpp$(PreprocessSuffix): bme280Controler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bme280Controler.cpp$(PreprocessSuffix) bme280Controler.cpp

$(IntermediateDirectory)/mqttClient.cpp$(ObjectSuffix): mqttClient.cpp $(IntermediateDirectory)/mqttClient.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/mqttClient.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mqttClient.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mqttClient.cpp$(DependSuffix): mqttClient.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mqttClient.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mqttClient.cpp$(DependSuffix) -MM mqttClient.cpp

$(IntermediateDirectory)/mqttClient.cpp$(PreprocessSuffix): mqttClient.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mqttClient.cpp$(PreprocessSuffix) mqttClient.cpp

$(IntermediateDirectory)/controler.cpp$(ObjectSuffix): controler.cpp $(IntermediateDirectory)/controler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/pi/cpp_projects/a291/a291unit/controler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/controler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/controler.cpp$(DependSuffix): controler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/controler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/controler.cpp$(DependSuffix) -MM controler.cpp

$(IntermediateDirectory)/controler.cpp$(PreprocessSuffix): controler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/controler.cpp$(PreprocessSuffix) controler.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


