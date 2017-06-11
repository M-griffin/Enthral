##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=enthral
ConfigurationName      :=Debug
WorkspacePath          :=/home/blue/code/Enthral/linux
ProjectPath            :=/home/blue/code/Enthral/linux/enthral
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=06/11/17
CodeLitePath           :=/home/blue/.codelite
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
ObjectsFileList        :="enthral.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../ $(IncludeSwitch)../src/jamlib $(IncludeSwitch)../../src/jamlib $(IncludeSwitch)../../../src/jamlib $(IncludeSwitch)/usr/include/python2.7 $(IncludeSwitch)/home/blue/code/Enthral/src/jamlib 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)ssl $(LibrarySwitch)crypto $(LibrarySwitch)pthread $(LibrarySwitch)boost_system $(LibrarySwitch)boost_filesystem $(LibrarySwitch)boost_python $(LibrarySwitch)util $(LibrarySwitch)python2.7 
ArLibs                 :=  "ssl" "crypto" "pthread" "boost_system" "boost_filesystem" "boost_python" "util" "python2.7" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)../src/jamlib $(LibraryPathSwitch)../../src/jamlib $(LibraryPathSwitch)../../../src/jamlib $(LibraryPathSwitch)/home/blue/code/Enthral/src/jamlib $(LibraryPathSwitch)/usr/lib/python2.7 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -pedantic -Wall -U_FORTIFY_SOURCE  -std=c++0x -fpermissive -D__LINUX__ $(Preprocessors)
CFLAGS   :=  -g -O0 -pedantic -Wall -U_FORTIFY_SOURCE  -std=c++0x -fpermissive -D__LINUX__ $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/up_up_src_configbbs.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_conio.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_console.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_data_areas.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_dtfunc.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_enthral.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_file_area.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_file_edit.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_file_list.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_files.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_src_history.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_language.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_logon.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_main_system.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_mb_api.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_mb_jam.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_menu_func.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_area.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_edit.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_email.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_pack.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_quote.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_read.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_readll.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_stats.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msg_title.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_msgs.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_node.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_user_list.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_users.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_up_src_configbbs.cpp$(ObjectSuffix): ../../src/configbbs.cpp $(IntermediateDirectory)/up_up_src_configbbs.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/configbbs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_configbbs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_configbbs.cpp$(DependSuffix): ../../src/configbbs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_configbbs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_configbbs.cpp$(DependSuffix) -MM ../../src/configbbs.cpp

$(IntermediateDirectory)/up_up_src_configbbs.cpp$(PreprocessSuffix): ../../src/configbbs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_configbbs.cpp$(PreprocessSuffix) ../../src/configbbs.cpp

$(IntermediateDirectory)/up_up_src_conio.cpp$(ObjectSuffix): ../../src/conio.cpp $(IntermediateDirectory)/up_up_src_conio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/conio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_conio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_conio.cpp$(DependSuffix): ../../src/conio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_conio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_conio.cpp$(DependSuffix) -MM ../../src/conio.cpp

$(IntermediateDirectory)/up_up_src_conio.cpp$(PreprocessSuffix): ../../src/conio.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_conio.cpp$(PreprocessSuffix) ../../src/conio.cpp

$(IntermediateDirectory)/up_up_src_console.cpp$(ObjectSuffix): ../../src/console.cpp $(IntermediateDirectory)/up_up_src_console.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/console.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_console.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_console.cpp$(DependSuffix): ../../src/console.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_console.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_console.cpp$(DependSuffix) -MM ../../src/console.cpp

$(IntermediateDirectory)/up_up_src_console.cpp$(PreprocessSuffix): ../../src/console.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_console.cpp$(PreprocessSuffix) ../../src/console.cpp

$(IntermediateDirectory)/up_up_src_data_areas.cpp$(ObjectSuffix): ../../src/data_areas.cpp $(IntermediateDirectory)/up_up_src_data_areas.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/data_areas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_data_areas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_data_areas.cpp$(DependSuffix): ../../src/data_areas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_data_areas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_data_areas.cpp$(DependSuffix) -MM ../../src/data_areas.cpp

$(IntermediateDirectory)/up_up_src_data_areas.cpp$(PreprocessSuffix): ../../src/data_areas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_data_areas.cpp$(PreprocessSuffix) ../../src/data_areas.cpp

$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(ObjectSuffix): ../../src/dtfunc.cpp $(IntermediateDirectory)/up_up_src_dtfunc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/dtfunc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(DependSuffix): ../../src/dtfunc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(DependSuffix) -MM ../../src/dtfunc.cpp

$(IntermediateDirectory)/up_up_src_dtfunc.cpp$(PreprocessSuffix): ../../src/dtfunc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_dtfunc.cpp$(PreprocessSuffix) ../../src/dtfunc.cpp

$(IntermediateDirectory)/up_up_src_enthral.cpp$(ObjectSuffix): ../../src/enthral.cpp $(IntermediateDirectory)/up_up_src_enthral.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/enthral.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_enthral.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_enthral.cpp$(DependSuffix): ../../src/enthral.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_enthral.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_enthral.cpp$(DependSuffix) -MM ../../src/enthral.cpp

$(IntermediateDirectory)/up_up_src_enthral.cpp$(PreprocessSuffix): ../../src/enthral.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_enthral.cpp$(PreprocessSuffix) ../../src/enthral.cpp

$(IntermediateDirectory)/up_up_src_file_area.cpp$(ObjectSuffix): ../../src/file_area.cpp $(IntermediateDirectory)/up_up_src_file_area.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/file_area.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_file_area.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_file_area.cpp$(DependSuffix): ../../src/file_area.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_file_area.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_file_area.cpp$(DependSuffix) -MM ../../src/file_area.cpp

$(IntermediateDirectory)/up_up_src_file_area.cpp$(PreprocessSuffix): ../../src/file_area.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_file_area.cpp$(PreprocessSuffix) ../../src/file_area.cpp

$(IntermediateDirectory)/up_up_src_file_edit.cpp$(ObjectSuffix): ../../src/file_edit.cpp $(IntermediateDirectory)/up_up_src_file_edit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/file_edit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_file_edit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_file_edit.cpp$(DependSuffix): ../../src/file_edit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_file_edit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_file_edit.cpp$(DependSuffix) -MM ../../src/file_edit.cpp

$(IntermediateDirectory)/up_up_src_file_edit.cpp$(PreprocessSuffix): ../../src/file_edit.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_file_edit.cpp$(PreprocessSuffix) ../../src/file_edit.cpp

$(IntermediateDirectory)/up_up_src_file_list.cpp$(ObjectSuffix): ../../src/file_list.cpp $(IntermediateDirectory)/up_up_src_file_list.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/file_list.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_file_list.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_file_list.cpp$(DependSuffix): ../../src/file_list.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_file_list.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_file_list.cpp$(DependSuffix) -MM ../../src/file_list.cpp

$(IntermediateDirectory)/up_up_src_file_list.cpp$(PreprocessSuffix): ../../src/file_list.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_file_list.cpp$(PreprocessSuffix) ../../src/file_list.cpp

$(IntermediateDirectory)/up_up_src_files.cpp$(ObjectSuffix): ../../src/files.cpp $(IntermediateDirectory)/up_up_src_files.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/files.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_files.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_files.cpp$(DependSuffix): ../../src/files.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_files.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_files.cpp$(DependSuffix) -MM ../../src/files.cpp

$(IntermediateDirectory)/up_up_src_files.cpp$(PreprocessSuffix): ../../src/files.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_files.cpp$(PreprocessSuffix) ../../src/files.cpp

$(IntermediateDirectory)/up_up_src_history.cpp$(ObjectSuffix): ../../src/history.cpp $(IntermediateDirectory)/up_up_src_history.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/history.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_history.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_history.cpp$(DependSuffix): ../../src/history.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_history.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_history.cpp$(DependSuffix) -MM ../../src/history.cpp

$(IntermediateDirectory)/up_up_src_history.cpp$(PreprocessSuffix): ../../src/history.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_history.cpp$(PreprocessSuffix) ../../src/history.cpp

$(IntermediateDirectory)/up_up_src_language.cpp$(ObjectSuffix): ../../src/language.cpp $(IntermediateDirectory)/up_up_src_language.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/language.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_language.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_language.cpp$(DependSuffix): ../../src/language.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_language.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_language.cpp$(DependSuffix) -MM ../../src/language.cpp

$(IntermediateDirectory)/up_up_src_language.cpp$(PreprocessSuffix): ../../src/language.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_language.cpp$(PreprocessSuffix) ../../src/language.cpp

$(IntermediateDirectory)/up_up_src_logon.cpp$(ObjectSuffix): ../../src/logon.cpp $(IntermediateDirectory)/up_up_src_logon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/logon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_logon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_logon.cpp$(DependSuffix): ../../src/logon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_logon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_logon.cpp$(DependSuffix) -MM ../../src/logon.cpp

$(IntermediateDirectory)/up_up_src_logon.cpp$(PreprocessSuffix): ../../src/logon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_logon.cpp$(PreprocessSuffix) ../../src/logon.cpp

$(IntermediateDirectory)/up_up_src_main_system.cpp$(ObjectSuffix): ../../src/main_system.cpp $(IntermediateDirectory)/up_up_src_main_system.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/main_system.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_main_system.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_main_system.cpp$(DependSuffix): ../../src/main_system.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_main_system.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_main_system.cpp$(DependSuffix) -MM ../../src/main_system.cpp

$(IntermediateDirectory)/up_up_src_main_system.cpp$(PreprocessSuffix): ../../src/main_system.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_main_system.cpp$(PreprocessSuffix) ../../src/main_system.cpp

$(IntermediateDirectory)/up_up_src_mb_api.cpp$(ObjectSuffix): ../../src/mb_api.cpp $(IntermediateDirectory)/up_up_src_mb_api.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/mb_api.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_mb_api.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_mb_api.cpp$(DependSuffix): ../../src/mb_api.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_mb_api.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_mb_api.cpp$(DependSuffix) -MM ../../src/mb_api.cpp

$(IntermediateDirectory)/up_up_src_mb_api.cpp$(PreprocessSuffix): ../../src/mb_api.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_mb_api.cpp$(PreprocessSuffix) ../../src/mb_api.cpp

$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(ObjectSuffix): ../../src/mb_jam.cpp $(IntermediateDirectory)/up_up_src_mb_jam.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/mb_jam.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(DependSuffix): ../../src/mb_jam.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(DependSuffix) -MM ../../src/mb_jam.cpp

$(IntermediateDirectory)/up_up_src_mb_jam.cpp$(PreprocessSuffix): ../../src/mb_jam.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_mb_jam.cpp$(PreprocessSuffix) ../../src/mb_jam.cpp

$(IntermediateDirectory)/up_up_src_menu_func.cpp$(ObjectSuffix): ../../src/menu_func.cpp $(IntermediateDirectory)/up_up_src_menu_func.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/menu_func.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_menu_func.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_menu_func.cpp$(DependSuffix): ../../src/menu_func.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_menu_func.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_menu_func.cpp$(DependSuffix) -MM ../../src/menu_func.cpp

$(IntermediateDirectory)/up_up_src_menu_func.cpp$(PreprocessSuffix): ../../src/menu_func.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_menu_func.cpp$(PreprocessSuffix) ../../src/menu_func.cpp

$(IntermediateDirectory)/up_up_src_msg_area.cpp$(ObjectSuffix): ../../src/msg_area.cpp $(IntermediateDirectory)/up_up_src_msg_area.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_area.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_area.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_area.cpp$(DependSuffix): ../../src/msg_area.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_area.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_area.cpp$(DependSuffix) -MM ../../src/msg_area.cpp

$(IntermediateDirectory)/up_up_src_msg_area.cpp$(PreprocessSuffix): ../../src/msg_area.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_area.cpp$(PreprocessSuffix) ../../src/msg_area.cpp

$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(ObjectSuffix): ../../src/msg_edit.cpp $(IntermediateDirectory)/up_up_src_msg_edit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_edit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(DependSuffix): ../../src/msg_edit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(DependSuffix) -MM ../../src/msg_edit.cpp

$(IntermediateDirectory)/up_up_src_msg_edit.cpp$(PreprocessSuffix): ../../src/msg_edit.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_edit.cpp$(PreprocessSuffix) ../../src/msg_edit.cpp

$(IntermediateDirectory)/up_up_src_msg_email.cpp$(ObjectSuffix): ../../src/msg_email.cpp $(IntermediateDirectory)/up_up_src_msg_email.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_email.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_email.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_email.cpp$(DependSuffix): ../../src/msg_email.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_email.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_email.cpp$(DependSuffix) -MM ../../src/msg_email.cpp

$(IntermediateDirectory)/up_up_src_msg_email.cpp$(PreprocessSuffix): ../../src/msg_email.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_email.cpp$(PreprocessSuffix) ../../src/msg_email.cpp

$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(ObjectSuffix): ../../src/msg_fse.cpp $(IntermediateDirectory)/up_up_src_msg_fse.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_fse.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(DependSuffix): ../../src/msg_fse.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(DependSuffix) -MM ../../src/msg_fse.cpp

$(IntermediateDirectory)/up_up_src_msg_fse.cpp$(PreprocessSuffix): ../../src/msg_fse.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_fse.cpp$(PreprocessSuffix) ../../src/msg_fse.cpp

$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(ObjectSuffix): ../../src/msg_newscan.cpp $(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_newscan.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(DependSuffix): ../../src/msg_newscan.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(DependSuffix) -MM ../../src/msg_newscan.cpp

$(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(PreprocessSuffix): ../../src/msg_newscan.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_newscan.cpp$(PreprocessSuffix) ../../src/msg_newscan.cpp

$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(ObjectSuffix): ../../src/msg_pack.cpp $(IntermediateDirectory)/up_up_src_msg_pack.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_pack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(DependSuffix): ../../src/msg_pack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(DependSuffix) -MM ../../src/msg_pack.cpp

$(IntermediateDirectory)/up_up_src_msg_pack.cpp$(PreprocessSuffix): ../../src/msg_pack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_pack.cpp$(PreprocessSuffix) ../../src/msg_pack.cpp

$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(ObjectSuffix): ../../src/msg_quote.cpp $(IntermediateDirectory)/up_up_src_msg_quote.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_quote.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(DependSuffix): ../../src/msg_quote.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(DependSuffix) -MM ../../src/msg_quote.cpp

$(IntermediateDirectory)/up_up_src_msg_quote.cpp$(PreprocessSuffix): ../../src/msg_quote.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_quote.cpp$(PreprocessSuffix) ../../src/msg_quote.cpp

$(IntermediateDirectory)/up_up_src_msg_read.cpp$(ObjectSuffix): ../../src/msg_read.cpp $(IntermediateDirectory)/up_up_src_msg_read.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_read.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_read.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_read.cpp$(DependSuffix): ../../src/msg_read.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_read.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_read.cpp$(DependSuffix) -MM ../../src/msg_read.cpp

$(IntermediateDirectory)/up_up_src_msg_read.cpp$(PreprocessSuffix): ../../src/msg_read.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_read.cpp$(PreprocessSuffix) ../../src/msg_read.cpp

$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(ObjectSuffix): ../../src/msg_readll.cpp $(IntermediateDirectory)/up_up_src_msg_readll.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_readll.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(DependSuffix): ../../src/msg_readll.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(DependSuffix) -MM ../../src/msg_readll.cpp

$(IntermediateDirectory)/up_up_src_msg_readll.cpp$(PreprocessSuffix): ../../src/msg_readll.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_readll.cpp$(PreprocessSuffix) ../../src/msg_readll.cpp

$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(ObjectSuffix): ../../src/msg_stats.cpp $(IntermediateDirectory)/up_up_src_msg_stats.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_stats.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(DependSuffix): ../../src/msg_stats.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(DependSuffix) -MM ../../src/msg_stats.cpp

$(IntermediateDirectory)/up_up_src_msg_stats.cpp$(PreprocessSuffix): ../../src/msg_stats.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_stats.cpp$(PreprocessSuffix) ../../src/msg_stats.cpp

$(IntermediateDirectory)/up_up_src_msg_title.cpp$(ObjectSuffix): ../../src/msg_title.cpp $(IntermediateDirectory)/up_up_src_msg_title.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msg_title.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msg_title.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msg_title.cpp$(DependSuffix): ../../src/msg_title.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msg_title.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msg_title.cpp$(DependSuffix) -MM ../../src/msg_title.cpp

$(IntermediateDirectory)/up_up_src_msg_title.cpp$(PreprocessSuffix): ../../src/msg_title.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msg_title.cpp$(PreprocessSuffix) ../../src/msg_title.cpp

$(IntermediateDirectory)/up_up_src_msgs.cpp$(ObjectSuffix): ../../src/msgs.cpp $(IntermediateDirectory)/up_up_src_msgs.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/msgs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_msgs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_msgs.cpp$(DependSuffix): ../../src/msgs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_msgs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_msgs.cpp$(DependSuffix) -MM ../../src/msgs.cpp

$(IntermediateDirectory)/up_up_src_msgs.cpp$(PreprocessSuffix): ../../src/msgs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_msgs.cpp$(PreprocessSuffix) ../../src/msgs.cpp

$(IntermediateDirectory)/up_up_src_node.cpp$(ObjectSuffix): ../../src/node.cpp $(IntermediateDirectory)/up_up_src_node.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/node.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_node.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_node.cpp$(DependSuffix): ../../src/node.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_node.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_node.cpp$(DependSuffix) -MM ../../src/node.cpp

$(IntermediateDirectory)/up_up_src_node.cpp$(PreprocessSuffix): ../../src/node.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_node.cpp$(PreprocessSuffix) ../../src/node.cpp

$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(ObjectSuffix): ../../src/pyenthral.cpp $(IntermediateDirectory)/up_up_src_pyenthral.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/pyenthral.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(DependSuffix): ../../src/pyenthral.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(DependSuffix) -MM ../../src/pyenthral.cpp

$(IntermediateDirectory)/up_up_src_pyenthral.cpp$(PreprocessSuffix): ../../src/pyenthral.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_pyenthral.cpp$(PreprocessSuffix) ../../src/pyenthral.cpp

$(IntermediateDirectory)/up_up_src_user_list.cpp$(ObjectSuffix): ../../src/user_list.cpp $(IntermediateDirectory)/up_up_src_user_list.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/user_list.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_user_list.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_user_list.cpp$(DependSuffix): ../../src/user_list.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_user_list.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_user_list.cpp$(DependSuffix) -MM ../../src/user_list.cpp

$(IntermediateDirectory)/up_up_src_user_list.cpp$(PreprocessSuffix): ../../src/user_list.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_user_list.cpp$(PreprocessSuffix) ../../src/user_list.cpp

$(IntermediateDirectory)/up_up_src_users.cpp$(ObjectSuffix): ../../src/users.cpp $(IntermediateDirectory)/up_up_src_users.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/users.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_users.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_users.cpp$(DependSuffix): ../../src/users.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_users.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_users.cpp$(DependSuffix) -MM ../../src/users.cpp

$(IntermediateDirectory)/up_up_src_users.cpp$(PreprocessSuffix): ../../src/users.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_users.cpp$(PreprocessSuffix) ../../src/users.cpp

$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(ObjectSuffix): ../../src/jamlib/crc32.cpp $(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/crc32.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(DependSuffix): ../../src/jamlib/crc32.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(DependSuffix) -MM ../../src/jamlib/crc32.cpp

$(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(PreprocessSuffix): ../../src/jamlib/crc32.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_crc32.cpp$(PreprocessSuffix) ../../src/jamlib/crc32.cpp

$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(ObjectSuffix): ../../src/jamlib/lastread.cpp $(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/lastread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(DependSuffix): ../../src/jamlib/lastread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(DependSuffix) -MM ../../src/jamlib/lastread.cpp

$(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(PreprocessSuffix): ../../src/jamlib/lastread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_lastread.cpp$(PreprocessSuffix) ../../src/jamlib/lastread.cpp

$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(ObjectSuffix): ../../src/jamlib/mbase.cpp $(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/mbase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(DependSuffix): ../../src/jamlib/mbase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(DependSuffix) -MM ../../src/jamlib/mbase.cpp

$(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(PreprocessSuffix): ../../src/jamlib/mbase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_mbase.cpp$(PreprocessSuffix) ../../src/jamlib/mbase.cpp

$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(ObjectSuffix): ../../src/jamlib/message.cpp $(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/message.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(DependSuffix): ../../src/jamlib/message.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(DependSuffix) -MM ../../src/jamlib/message.cpp

$(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(PreprocessSuffix): ../../src/jamlib/message.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_message.cpp$(PreprocessSuffix) ../../src/jamlib/message.cpp

$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(ObjectSuffix): ../../src/jamlib/structrw.cpp $(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/structrw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(DependSuffix): ../../src/jamlib/structrw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(DependSuffix) -MM ../../src/jamlib/structrw.cpp

$(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(PreprocessSuffix): ../../src/jamlib/structrw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_structrw.cpp$(PreprocessSuffix) ../../src/jamlib/structrw.cpp

$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(ObjectSuffix): ../../src/jamlib/subpack.cpp $(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/blue/code/Enthral/src/jamlib/subpack.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(DependSuffix): ../../src/jamlib/subpack.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(DependSuffix) -MM ../../src/jamlib/subpack.cpp

$(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(PreprocessSuffix): ../../src/jamlib/subpack.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_src_jamlib_subpack.cpp$(PreprocessSuffix) ../../src/jamlib/subpack.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


