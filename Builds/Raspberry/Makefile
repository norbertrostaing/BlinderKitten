# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef PKG_CONFIG
  PKG_CONFIG=pkg-config
endif

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-D_ITERATOR_DEBUG_LEVEL=1" "-DLINK_PLATFORM_LINUX=1" "-DJUCER_LINUX_MAKE_9C1B7400=1" "-DJUCE_APP_VERSION=1.0.1b80" "-DJUCE_APP_VERSION_HEX=0x10001" $(shell $(PKG_CONFIG) --cflags alsa freetype2 gl libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../JUCE/modules -I../../Modules -I../../Source -I../../Source/Definitions -I../../External/serial/include -I../../External/servus/include -I../../External/dnssd/include -I../../External/ois/include -I../../External/sdl/include -I../../External/libusb/include/libusb-1.0 $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := BlinderKitten

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L../../Modules/juce_simpleweb/libs/Linux/${JUCE_ARCH_LABEL} $(shell $(PKG_CONFIG) --libs alsa freetype2 gl libcurl) -fvisibility=hidden -Wl,-rpath,"lib" -Wl,--as-needed -lrt -ldl -lpthread -lssl -lcrypto -lServus -lcurl -lusb-1.0 -lhidapi-hidraw -lpthread -latomic $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=armv8-a
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_MODAL_LOOPS_PERMITTED=1" "-D_ITERATOR_DEBUG_LEVEL=1" "-DLINK_PLATFORM_LINUX=1" "-DJUCER_LINUX_MAKE_9C1B7400=1" "-DJUCE_APP_VERSION=1.0.1b80" "-DJUCE_APP_VERSION_HEX=0x10001" $(shell $(PKG_CONFIG) --cflags alsa freetype2 gl libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../JUCE/modules -I../../Modules -I/usr/include/freetype2 -I../../Source -I../../Source/Definitions -I../../External/serial/include -I../../External/servus/include -I../../External/dnssd/include -I../../External/ois/include -I../../External/sdl/include -I../../External/libusb/include/libusb-1.0 $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := BlinderKitten

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L../../External/servus/lib/raspberry -L/usr/lib/arm-linux-gnueabihf -L../../Modules/juce_simpleweb/libs/Linux/armv8-a $(shell $(PKG_CONFIG) --libs alsa freetype2 gl libcurl) -fvisibility=hidden -Wl,-rpath,"lib" -Wl,--as-needed -lrt -ldl -lpthread -lssl -lcrypto -lServus -lcurl -lusb-1.0 -lhidapi-hidraw -lpthread -latomic $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/CuelistSheet_c446831d.o \
  $(JUCE_OBJDIR)/CuelistSheetLine_cec00931.o \
  $(JUCE_OBJDIR)/EncodersMult_c99223bd.o \
  $(JUCE_OBJDIR)/EncodersMultCmd_6c9aa0c7.o \
  $(JUCE_OBJDIR)/EncodersMultUI_ee98a251.o \
  $(JUCE_OBJDIR)/CarouselGridView_d08619bd.o \
  $(JUCE_OBJDIR)/CuelistGridView_8ef10494.o \
  $(JUCE_OBJDIR)/EffectGridView_6b28bae.o \
  $(JUCE_OBJDIR)/FixtureGridView_2501c94c.o \
  $(JUCE_OBJDIR)/GridView_42bfc13d.o \
  $(JUCE_OBJDIR)/GroupGridView_5fc98182.o \
  $(JUCE_OBJDIR)/MapperGridView_2ac6085e.o \
  $(JUCE_OBJDIR)/PresetGridView_d172ffc.o \
  $(JUCE_OBJDIR)/PatchSheet_2c03483d.o \
  $(JUCE_OBJDIR)/PatchSheetLine_d2553e51.o \
  $(JUCE_OBJDIR)/VirtualButton_57e0460.o \
  $(JUCE_OBJDIR)/VirtualButtonGrid_861b1346.o \
  $(JUCE_OBJDIR)/VirtualButtonManager_c5a2c3b7.o \
  $(JUCE_OBJDIR)/VirtualButtonManagerUI_29012bcb.o \
  $(JUCE_OBJDIR)/VirtualFaderButton_277ac860.o \
  $(JUCE_OBJDIR)/VirtualFaderCol_21a1e75c.o \
  $(JUCE_OBJDIR)/VirtualFaderColGrid_575d1842.o \
  $(JUCE_OBJDIR)/VirtualFaderColManager_5764cd3b.o \
  $(JUCE_OBJDIR)/VirtualFaderColManagerUI_5266e44f.o \
  $(JUCE_OBJDIR)/VirtualFaderSlider_540aaa8f.o \
  $(JUCE_OBJDIR)/BKColorPicker_26a4f576.o \
  $(JUCE_OBJDIR)/Clock_9b1dd19c.o \
  $(JUCE_OBJDIR)/ConductorInfos_9dff399a.o \
  $(JUCE_OBJDIR)/CuelistLoadWindow_9733d835.o \
  $(JUCE_OBJDIR)/DMXChannelView_4a36d235.o \
  $(JUCE_OBJDIR)/Encoders_6bf10993.o \
  $(JUCE_OBJDIR)/InputPanel_276fbbd6.o \
  $(JUCE_OBJDIR)/LabelAndTime_69a3566c.o \
  $(JUCE_OBJDIR)/LayoutViewer_b08c5698.o \
  $(JUCE_OBJDIR)/list_ports_linux_203b2613.o \
  $(JUCE_OBJDIR)/list_ports_osx_d93ade33.o \
  $(JUCE_OBJDIR)/list_ports_win_e6502c6b.o \
  $(JUCE_OBJDIR)/unix_d5f5c79d.o \
  $(JUCE_OBJDIR)/win_4c1bdbbb.o \
  $(JUCE_OBJDIR)/serial_82f97e54.o \
  $(JUCE_OBJDIR)/CommonIncludes_97d07afb.o \
  $(JUCE_OBJDIR)/BundleAction_100e7ca2.o \
  $(JUCE_OBJDIR)/CarouselAction_b771dec0.o \
  $(JUCE_OBJDIR)/CuelistAction_667db027.o \
  $(JUCE_OBJDIR)/EffectAction_c4a53ef1.o \
  $(JUCE_OBJDIR)/EncoderAction_90a75232.o \
  $(JUCE_OBJDIR)/GenericAction_be28692d.o \
  $(JUCE_OBJDIR)/InputPanelAction_a2d41bfa.o \
  $(JUCE_OBJDIR)/InterfaceAction_34f8234f.o \
  $(JUCE_OBJDIR)/MapperAction_7d027a1.o \
  $(JUCE_OBJDIR)/MultiplicatorAction_f579383b.o \
  $(JUCE_OBJDIR)/PresetAction_e4e29ebf.o \
  $(JUCE_OBJDIR)/SoloPoolAction_e45293b.o \
  $(JUCE_OBJDIR)/TapTempoMultiple_3a521832.o \
  $(JUCE_OBJDIR)/TrackerAction_c5bb14e.o \
  $(JUCE_OBJDIR)/VirtualButtonAction_1210c9b3.o \
  $(JUCE_OBJDIR)/VirtualFaderAction_b95e624b.o \
  $(JUCE_OBJDIR)/Assistant_feae32ff.o \
  $(JUCE_OBJDIR)/BKPathPreset_490b261.o \
  $(JUCE_OBJDIR)/BKPathPresetManager_82b18296.o \
  $(JUCE_OBJDIR)/BKPathPresetManagerUI_dd5baeea.o \
  $(JUCE_OBJDIR)/Bundle_9d272dc1.o \
  $(JUCE_OBJDIR)/BundleManager_5a5b7b36.o \
  $(JUCE_OBJDIR)/BundleManagerUI_7269ff8a.o \
  $(JUCE_OBJDIR)/BundleSelection_e70e5095.o \
  $(JUCE_OBJDIR)/BundleSelectionManager_5d0341e2.o \
  $(JUCE_OBJDIR)/Carousel_4fe581c1.o \
  $(JUCE_OBJDIR)/CarouselManager_9896a736.o \
  $(JUCE_OBJDIR)/CarouselManagerUI_e8a2b8a.o \
  $(JUCE_OBJDIR)/CarouselRow_2a173ce3.o \
  $(JUCE_OBJDIR)/CarouselStep_6e0340ad.o \
  $(JUCE_OBJDIR)/ChannelType_cb44b895.o \
  $(JUCE_OBJDIR)/ChannelFamily_c41891.o \
  $(JUCE_OBJDIR)/ChannelFamilyManager_2937c666.o \
  $(JUCE_OBJDIR)/ChannelFamilyManagerUI_fb603eba.o \
  $(JUCE_OBJDIR)/Command_ba8d2819.o \
  $(JUCE_OBJDIR)/CommandManager_ec8fadde.o \
  $(JUCE_OBJDIR)/CommandManagerUI_485c2832.o \
  $(JUCE_OBJDIR)/CommandSelection_bd00793d.o \
  $(JUCE_OBJDIR)/CommandSelectionManager_ad998c3a.o \
  $(JUCE_OBJDIR)/CommandTiming_d4942483.o \
  $(JUCE_OBJDIR)/CommandValue_d798fca2.o \
  $(JUCE_OBJDIR)/CommandValueManager_2dce5d35.o \
  $(JUCE_OBJDIR)/MoveInBlackManager_55053620.o \
  $(JUCE_OBJDIR)/Cue_a2480ce9.o \
  $(JUCE_OBJDIR)/CueManager_b280bf0e.o \
  $(JUCE_OBJDIR)/Task_85d81459.o \
  $(JUCE_OBJDIR)/Cuelist_6fc7e025.o \
  $(JUCE_OBJDIR)/CuelistManager_78e41052.o \
  $(JUCE_OBJDIR)/CuelistManagerUI_1121bda6.o \
  $(JUCE_OBJDIR)/CurvePreset_28c605f.o \
  $(JUCE_OBJDIR)/CurvePresetManager_2113bad8.o \
  $(JUCE_OBJDIR)/CurvePresetManagerUI_6c10deac.o \
  $(JUCE_OBJDIR)/DataTransferManager_61c34373.o \
  $(JUCE_OBJDIR)/Effect_8f971ee1.o \
  $(JUCE_OBJDIR)/EffectManager_94700616.o \
  $(JUCE_OBJDIR)/EffectManagerUI_7987526a.o \
  $(JUCE_OBJDIR)/EffectParam_7fbc9bf6.o \
  $(JUCE_OBJDIR)/EffectRow_48330bc3.o \
  $(JUCE_OBJDIR)/Fixture_975a195.o \
  $(JUCE_OBJDIR)/FixtureManager_47eed0e2.o \
  $(JUCE_OBJDIR)/FixtureManagerUI_487e9a36.o \
  $(JUCE_OBJDIR)/FixtureMapping_ff366183.o \
  $(JUCE_OBJDIR)/FixtureMultiEditor_727ec0bb.o \
  $(JUCE_OBJDIR)/FixturePatch_f6b75bfd.o \
  $(JUCE_OBJDIR)/FixturePatchCorrection_dd7e6e7b.o \
  $(JUCE_OBJDIR)/FixtureType_5fe79449.o \
  $(JUCE_OBJDIR)/FixtureTypeChannel_f51bfe04.o \
  $(JUCE_OBJDIR)/FixtureTypeChannelManager_c4b6993.o \
  $(JUCE_OBJDIR)/FixtureTypeManager_e71c2bae.o \
  $(JUCE_OBJDIR)/FixtureTypeManagerUI_d1c07202.o \
  $(JUCE_OBJDIR)/FixtureTypeVirtualChannel_8aa184e1.o \
  $(JUCE_OBJDIR)/FixtureTypeVirtualChannelManager_b206e016.o \
  $(JUCE_OBJDIR)/Group_3d0e0901.o \
  $(JUCE_OBJDIR)/GroupManager_c56377f6.o \
  $(JUCE_OBJDIR)/GroupManagerUI_3b65cc4a.o \
  $(JUCE_OBJDIR)/InterfaceIncludes_a0d5be60.o \
  $(JUCE_OBJDIR)/BKPath_ef1d6e62.o \
  $(JUCE_OBJDIR)/Layout_4da3e0c1.o \
  $(JUCE_OBJDIR)/LayoutManager_d8bf6836.o \
  $(JUCE_OBJDIR)/LayoutManagerUI_e786ac8a.o \
  $(JUCE_OBJDIR)/Mapper_c6bc78e1.o \
  $(JUCE_OBJDIR)/MapperManager_23716c16.o \
  $(JUCE_OBJDIR)/MapperManagerUI_4dc7386a.o \
  $(JUCE_OBJDIR)/MapperRow_abd771c3.o \
  $(JUCE_OBJDIR)/MapperStep_2449a7cd.o \
  $(JUCE_OBJDIR)/Multiplicator_13f1ecd.o \
  $(JUCE_OBJDIR)/MultiplicatorLink_61f7d9e7.o \
  $(JUCE_OBJDIR)/MultiplicatorLinkManager_411df850.o \
  $(JUCE_OBJDIR)/MultiplicatorManager_583084aa.o \
  $(JUCE_OBJDIR)/MultiplicatorManagerUI_4f227bfe.o \
  $(JUCE_OBJDIR)/Preset_69828b21.o \
  $(JUCE_OBJDIR)/PresetManager_c35e51d6.o \
  $(JUCE_OBJDIR)/PresetManagerUI_a611ae2a.o \
  $(JUCE_OBJDIR)/PresetSubFixtureValues_d6740bec.o \
  $(JUCE_OBJDIR)/PresetValue_37303e9a.o \
  $(JUCE_OBJDIR)/Programmer_37f4f5c1.o \
  $(JUCE_OBJDIR)/ProgrammerManager_59fcb336.o \
  $(JUCE_OBJDIR)/ProgrammerManagerUI_e9d378a.o \
  $(JUCE_OBJDIR)/SubFixture_8e66f3e1.o \
  $(JUCE_OBJDIR)/SubFixtureChannel_8716636c.o \
  $(JUCE_OBJDIR)/TimingPreset_47a547e1.o \
  $(JUCE_OBJDIR)/TimingPresetManager_60d1bd16.o \
  $(JUCE_OBJDIR)/TimingPresetManagerUI_b457496a.o \
  $(JUCE_OBJDIR)/Tracker_764dddf3.o \
  $(JUCE_OBJDIR)/TrackerManager_23498ec4.o \
  $(JUCE_OBJDIR)/TrackerManagerUI_b8216798.o \
  $(JUCE_OBJDIR)/ChannelValue_6d2d5a86.o \
  $(JUCE_OBJDIR)/RunningTask_1a6b9796.o \
  $(JUCE_OBJDIR)/BKEngine_e62bccd4.o \
  $(JUCE_OBJDIR)/Brain_6743ab57.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/MainComponentCommands_b1179f4d.o \
  $(JUCE_OBJDIR)/mainIncludes_cf5c0d.o \
  $(JUCE_OBJDIR)/UserInputManager_30ae3f77.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \
  $(JUCE_OBJDIR)/include_juce_organicui_2c36794d.o \
  $(JUCE_OBJDIR)/include_juce_organicui2_5ab8c64f.o \
  $(JUCE_OBJDIR)/include_juce_organicui2b_ff2481bd.o \
  $(JUCE_OBJDIR)/include_juce_organicui3_5ac6ddd0.o \
  $(JUCE_OBJDIR)/include_juce_organicui4_5ad4f551.o \
  $(JUCE_OBJDIR)/include_juce_osc_f3df604d.o \
  $(JUCE_OBJDIR)/include_juce_simpleweb_acec7570.o \
  $(JUCE_OBJDIR)/include_juce_simpleweb2_f0c24e8c.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(JUCE_OBJDIR)/execinfo.cmd $(RESOURCES)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors alsa freetype2 gl libcurl
	@echo Linking "BlinderKitten - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(shell cat $(JUCE_OBJDIR)/execinfo.cmd) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/CuelistSheet_c446831d.o: ../../Source/UI/CuelistSheet/CuelistSheet.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistSheet.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistSheetLine_cec00931.o: ../../Source/UI/CuelistSheet/CuelistSheetLine.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistSheetLine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EncodersMult_c99223bd.o: ../../Source/UI/EncodersMult/EncodersMult.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EncodersMult.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EncodersMultCmd_6c9aa0c7.o: ../../Source/UI/EncodersMult/EncodersMultCmd.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EncodersMultCmd.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EncodersMultUI_ee98a251.o: ../../Source/UI/EncodersMult/EncodersMultUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EncodersMultUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselGridView_d08619bd.o: ../../Source/UI/GridView/CarouselGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistGridView_8ef10494.o: ../../Source/UI/GridView/CuelistGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectGridView_6b28bae.o: ../../Source/UI/GridView/EffectGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureGridView_2501c94c.o: ../../Source/UI/GridView/FixtureGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GridView_42bfc13d.o: ../../Source/UI/GridView/GridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GroupGridView_5fc98182.o: ../../Source/UI/GridView/GroupGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GroupGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperGridView_2ac6085e.o: ../../Source/UI/GridView/MapperGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetGridView_d172ffc.o: ../../Source/UI/GridView/PresetGridView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetGridView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatchSheet_2c03483d.o: ../../Source/UI/PatchSheet/PatchSheet.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PatchSheet.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatchSheetLine_d2553e51.o: ../../Source/UI/PatchSheet/PatchSheetLine.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PatchSheetLine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualButton_57e0460.o: ../../Source/UI/VirtualButtons/VirtualButton.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualButtonGrid_861b1346.o: ../../Source/UI/VirtualButtons/VirtualButtonGrid.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualButtonGrid.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualButtonManager_c5a2c3b7.o: ../../Source/UI/VirtualButtons/VirtualButtonManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualButtonManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualButtonManagerUI_29012bcb.o: ../../Source/UI/VirtualButtons/VirtualButtonManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualButtonManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderButton_277ac860.o: ../../Source/UI/VirtualFaders/VirtualFaderButton.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderCol_21a1e75c.o: ../../Source/UI/VirtualFaders/VirtualFaderCol.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderCol.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderColGrid_575d1842.o: ../../Source/UI/VirtualFaders/VirtualFaderColGrid.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderColGrid.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderColManager_5764cd3b.o: ../../Source/UI/VirtualFaders/VirtualFaderColManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderColManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderColManagerUI_5266e44f.o: ../../Source/UI/VirtualFaders/VirtualFaderColManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderColManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderSlider_540aaa8f.o: ../../Source/UI/VirtualFaders/VirtualFaderSlider.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderSlider.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKColorPicker_26a4f576.o: ../../Source/UI/BKColorPicker.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKColorPicker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Clock_9b1dd19c.o: ../../Source/UI/Clock.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Clock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ConductorInfos_9dff399a.o: ../../Source/UI/ConductorInfos.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ConductorInfos.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistLoadWindow_9733d835.o: ../../Source/UI/CuelistLoadWindow.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistLoadWindow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DMXChannelView_4a36d235.o: ../../Source/UI/DMXChannelView.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DMXChannelView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Encoders_6bf10993.o: ../../Source/UI/Encoders.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Encoders.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InputPanel_276fbbd6.o: ../../Source/UI/InputPanel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling InputPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LabelAndTime_69a3566c.o: ../../Source/UI/LabelAndTime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LabelAndTime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LayoutViewer_b08c5698.o: ../../Source/UI/LayoutViewer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LayoutViewer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_linux_203b2613.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_linux.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling list_ports_linux.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_osx_d93ade33.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_osx.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling list_ports_osx.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_win_e6502c6b.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_win.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling list_ports_win.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/unix_d5f5c79d.o: ../../Source/Common/Serial/lib/serial/impl/unix.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling unix.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/win_4c1bdbbb.o: ../../Source/Common/Serial/lib/serial/impl/win.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling win.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/serial_82f97e54.o: ../../Source/Common/Serial/lib/serial/serial.cc
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling serial.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommonIncludes_97d07afb.o: ../../Source/Common/CommonIncludes.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommonIncludes.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BundleAction_100e7ca2.o: ../../Source/Definitions/Actions/BundleAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BundleAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselAction_b771dec0.o: ../../Source/Definitions/Actions/CarouselAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistAction_667db027.o: ../../Source/Definitions/Actions/CuelistAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectAction_c4a53ef1.o: ../../Source/Definitions/Actions/EffectAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EncoderAction_90a75232.o: ../../Source/Definitions/Actions/EncoderAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EncoderAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GenericAction_be28692d.o: ../../Source/Definitions/Actions/GenericAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GenericAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InputPanelAction_a2d41bfa.o: ../../Source/Definitions/Actions/InputPanelAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling InputPanelAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InterfaceAction_34f8234f.o: ../../Source/Definitions/Actions/InterfaceAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling InterfaceAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperAction_7d027a1.o: ../../Source/Definitions/Actions/MapperAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiplicatorAction_f579383b.o: ../../Source/Definitions/Actions/MultiplicatorAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MultiplicatorAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetAction_e4e29ebf.o: ../../Source/Definitions/Actions/PresetAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SoloPoolAction_e45293b.o: ../../Source/Definitions/Actions/SoloPoolAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SoloPoolAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TapTempoMultiple_3a521832.o: ../../Source/Definitions/Actions/TapTempoMultiple.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TapTempoMultiple.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackerAction_c5bb14e.o: ../../Source/Definitions/Actions/TrackerAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TrackerAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualButtonAction_1210c9b3.o: ../../Source/Definitions/Actions/VirtualButtonAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualButtonAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VirtualFaderAction_b95e624b.o: ../../Source/Definitions/Actions/VirtualFaderAction.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VirtualFaderAction.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Assistant_feae32ff.o: ../../Source/Definitions/Assistant/Assistant.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Assistant.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKPathPreset_490b261.o: ../../Source/Definitions/BKPathPreset/BKPathPreset.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKPathPreset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKPathPresetManager_82b18296.o: ../../Source/Definitions/BKPathPreset/BKPathPresetManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKPathPresetManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKPathPresetManagerUI_dd5baeea.o: ../../Source/Definitions/BKPathPreset/BKPathPresetManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKPathPresetManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Bundle_9d272dc1.o: ../../Source/Definitions/Bundle/Bundle.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Bundle.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BundleManager_5a5b7b36.o: ../../Source/Definitions/Bundle/BundleManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BundleManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BundleManagerUI_7269ff8a.o: ../../Source/Definitions/Bundle/BundleManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BundleManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BundleSelection_e70e5095.o: ../../Source/Definitions/Bundle/BundleSelection.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BundleSelection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BundleSelectionManager_5d0341e2.o: ../../Source/Definitions/Bundle/BundleSelectionManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BundleSelectionManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Carousel_4fe581c1.o: ../../Source/Definitions/Carousel/Carousel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Carousel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselManager_9896a736.o: ../../Source/Definitions/Carousel/CarouselManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselManagerUI_e8a2b8a.o: ../../Source/Definitions/Carousel/CarouselManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselRow_2a173ce3.o: ../../Source/Definitions/Carousel/CarouselRow.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CarouselStep_6e0340ad.o: ../../Source/Definitions/Carousel/CarouselStep.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CarouselStep.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChannelType_cb44b895.o: ../../Source/Definitions/ChannelFamily/ChannelType/ChannelType.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChannelType.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChannelFamily_c41891.o: ../../Source/Definitions/ChannelFamily/ChannelFamily.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChannelFamily.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChannelFamilyManager_2937c666.o: ../../Source/Definitions/ChannelFamily/ChannelFamilyManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChannelFamilyManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChannelFamilyManagerUI_fb603eba.o: ../../Source/Definitions/ChannelFamily/ChannelFamilyManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChannelFamilyManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Command_ba8d2819.o: ../../Source/Definitions/Command/Command.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Command.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandManager_ec8fadde.o: ../../Source/Definitions/Command/CommandManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandManagerUI_485c2832.o: ../../Source/Definitions/Command/CommandManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandSelection_bd00793d.o: ../../Source/Definitions/Command/CommandSelection.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandSelection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandSelectionManager_ad998c3a.o: ../../Source/Definitions/Command/CommandSelectionManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandSelectionManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandTiming_d4942483.o: ../../Source/Definitions/Command/CommandTiming.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandTiming.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandValue_d798fca2.o: ../../Source/Definitions/Command/CommandValue.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandValue.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandValueManager_2dce5d35.o: ../../Source/Definitions/Command/CommandValueManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CommandValueManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MoveInBlackManager_55053620.o: ../../Source/Definitions/Command/MoveInBlackManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MoveInBlackManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Cue_a2480ce9.o: ../../Source/Definitions/Cue/Cue.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Cue.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CueManager_b280bf0e.o: ../../Source/Definitions/Cue/CueManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CueManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Task_85d81459.o: ../../Source/Definitions/Cue/Task.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Task.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Cuelist_6fc7e025.o: ../../Source/Definitions/Cuelist/Cuelist.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Cuelist.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistManager_78e41052.o: ../../Source/Definitions/Cuelist/CuelistManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CuelistManagerUI_1121bda6.o: ../../Source/Definitions/Cuelist/CuelistManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CuelistManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CurvePreset_28c605f.o: ../../Source/Definitions/CurvePreset/CurvePreset.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CurvePreset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CurvePresetManager_2113bad8.o: ../../Source/Definitions/CurvePreset/CurvePresetManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CurvePresetManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CurvePresetManagerUI_6c10deac.o: ../../Source/Definitions/CurvePreset/CurvePresetManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CurvePresetManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DataTransferManager_61c34373.o: ../../Source/Definitions/DataTransferManager/DataTransferManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DataTransferManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Effect_8f971ee1.o: ../../Source/Definitions/Effect/Effect.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Effect.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectManager_94700616.o: ../../Source/Definitions/Effect/EffectManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectManagerUI_7987526a.o: ../../Source/Definitions/Effect/EffectManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectParam_7fbc9bf6.o: ../../Source/Definitions/Effect/EffectParam.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectParam.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/EffectRow_48330bc3.o: ../../Source/Definitions/Effect/EffectRow.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling EffectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Fixture_975a195.o: ../../Source/Definitions/Fixture/Fixture.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Fixture.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureManager_47eed0e2.o: ../../Source/Definitions/Fixture/FixtureManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureManagerUI_487e9a36.o: ../../Source/Definitions/Fixture/FixtureManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureMapping_ff366183.o: ../../Source/Definitions/Fixture/FixtureMapping.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureMapping.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureMultiEditor_727ec0bb.o: ../../Source/Definitions/Fixture/FixtureMultiEditor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureMultiEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixturePatch_f6b75bfd.o: ../../Source/Definitions/Fixture/FixturePatch.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixturePatch.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixturePatchCorrection_dd7e6e7b.o: ../../Source/Definitions/Fixture/FixturePatchCorrection.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixturePatchCorrection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureType_5fe79449.o: ../../Source/Definitions/FixtureType/FixtureType.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureType.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeChannel_f51bfe04.o: ../../Source/Definitions/FixtureType/FixtureTypeChannel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeChannel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeChannelManager_c4b6993.o: ../../Source/Definitions/FixtureType/FixtureTypeChannelManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeChannelManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeManager_e71c2bae.o: ../../Source/Definitions/FixtureType/FixtureTypeManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeManagerUI_d1c07202.o: ../../Source/Definitions/FixtureType/FixtureTypeManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeVirtualChannel_8aa184e1.o: ../../Source/Definitions/FixtureType/FixtureTypeVirtualChannel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeVirtualChannel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FixtureTypeVirtualChannelManager_b206e016.o: ../../Source/Definitions/FixtureType/FixtureTypeVirtualChannelManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FixtureTypeVirtualChannelManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Group_3d0e0901.o: ../../Source/Definitions/Group/Group.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Group.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GroupManager_c56377f6.o: ../../Source/Definitions/Group/GroupManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GroupManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GroupManagerUI_3b65cc4a.o: ../../Source/Definitions/Group/GroupManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling GroupManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InterfaceIncludes_a0d5be60.o: ../../Source/Definitions/Interface/InterfaceIncludes.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling InterfaceIncludes.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKPath_ef1d6e62.o: ../../Source/Definitions/Layout/BKPath/BKPath.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKPath.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Layout_4da3e0c1.o: ../../Source/Definitions/Layout/Layout.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Layout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LayoutManager_d8bf6836.o: ../../Source/Definitions/Layout/LayoutManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LayoutManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LayoutManagerUI_e786ac8a.o: ../../Source/Definitions/Layout/LayoutManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LayoutManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Mapper_c6bc78e1.o: ../../Source/Definitions/Mapper/Mapper.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Mapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperManager_23716c16.o: ../../Source/Definitions/Mapper/MapperManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperManagerUI_4dc7386a.o: ../../Source/Definitions/Mapper/MapperManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperRow_abd771c3.o: ../../Source/Definitions/Mapper/MapperRow.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MapperStep_2449a7cd.o: ../../Source/Definitions/Mapper/MapperStep.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MapperStep.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Multiplicator_13f1ecd.o: ../../Source/Definitions/Multiplicator/Multiplicator.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Multiplicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiplicatorLink_61f7d9e7.o: ../../Source/Definitions/Multiplicator/MultiplicatorLink.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MultiplicatorLink.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiplicatorLinkManager_411df850.o: ../../Source/Definitions/Multiplicator/MultiplicatorLinkManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MultiplicatorLinkManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiplicatorManager_583084aa.o: ../../Source/Definitions/Multiplicator/MultiplicatorManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MultiplicatorManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiplicatorManagerUI_4f227bfe.o: ../../Source/Definitions/Multiplicator/MultiplicatorManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MultiplicatorManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Preset_69828b21.o: ../../Source/Definitions/Preset/Preset.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Preset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetManager_c35e51d6.o: ../../Source/Definitions/Preset/PresetManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetManagerUI_a611ae2a.o: ../../Source/Definitions/Preset/PresetManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetSubFixtureValues_d6740bec.o: ../../Source/Definitions/Preset/PresetSubFixtureValues.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetSubFixtureValues.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PresetValue_37303e9a.o: ../../Source/Definitions/Preset/PresetValue.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PresetValue.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Programmer_37f4f5c1.o: ../../Source/Definitions/Programmer/Programmer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Programmer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProgrammerManager_59fcb336.o: ../../Source/Definitions/Programmer/ProgrammerManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ProgrammerManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProgrammerManagerUI_e9d378a.o: ../../Source/Definitions/Programmer/ProgrammerManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ProgrammerManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SubFixture_8e66f3e1.o: ../../Source/Definitions/SubFixture/SubFixture.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SubFixture.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SubFixtureChannel_8716636c.o: ../../Source/Definitions/SubFixture/SubFixtureChannel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SubFixtureChannel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimingPreset_47a547e1.o: ../../Source/Definitions/TimingPreset/TimingPreset.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TimingPreset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimingPresetManager_60d1bd16.o: ../../Source/Definitions/TimingPreset/TimingPresetManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TimingPresetManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimingPresetManagerUI_b457496a.o: ../../Source/Definitions/TimingPreset/TimingPresetManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TimingPresetManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Tracker_764dddf3.o: ../../Source/Definitions/Tracker/Tracker.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Tracker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackerManager_23498ec4.o: ../../Source/Definitions/Tracker/TrackerManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TrackerManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackerManagerUI_b8216798.o: ../../Source/Definitions/Tracker/TrackerManagerUI.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TrackerManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChannelValue_6d2d5a86.o: ../../Source/Definitions/ChannelValue.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChannelValue.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RunningTask_1a6b9796.o: ../../Source/Definitions/RunningTask.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling RunningTask.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BKEngine_e62bccd4.o: ../../Source/BKEngine.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BKEngine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Brain_6743ab57.o: ../../Source/Brain.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Brain.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponentCommands_b1179f4d.o: ../../Source/MainComponentCommands.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainComponentCommands.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/mainIncludes_cf5c0d.o: ../../Source/mainIncludes.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling mainIncludes.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserInputManager_30ae3f77.o: ../../Source/UserInputManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling UserInputManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o: ../../JuceLibraryCode/include_juce_audio_processors_ara.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_ara.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o: ../../JuceLibraryCode/include_juce_audio_processors_lv2_libs.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_lv2_libs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o: ../../JuceLibraryCode/include_juce_cryptography.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_cryptography.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui_2c36794d.o: ../../JuceLibraryCode/include_juce_organicui.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_organicui.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui2_5ab8c64f.o: ../../JuceLibraryCode/include_juce_organicui2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_organicui2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui2b_ff2481bd.o: ../../JuceLibraryCode/include_juce_organicui2b.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_organicui2b.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui3_5ac6ddd0.o: ../../JuceLibraryCode/include_juce_organicui3.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_organicui3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui4_5ad4f551.o: ../../JuceLibraryCode/include_juce_organicui4.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_organicui4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_osc_f3df604d.o: ../../JuceLibraryCode/include_juce_osc.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_osc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_simpleweb_acec7570.o: ../../JuceLibraryCode/include_juce_simpleweb.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_simpleweb.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_simpleweb2_f0c24e8c.o: ../../JuceLibraryCode/include_juce_simpleweb2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_simpleweb2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/execinfo.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link libexecinfo"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/execinfo.x -lexecinfo - >/dev/null 2>&1 && printf -- "-lexecinfo" > "$@" || touch "$@"

$(JUCE_OBJDIR)/cxxfs.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link stdc++fs"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/cxxfs.x -lstdc++fs - >/dev/null 2>&1 && printf -- "-lstdc++fs" > "$@" || touch "$@"

clean:
	@echo Cleaning BlinderKitten
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping BlinderKitten
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

-include $(OBJECTS_APP:%.o=%.d)
