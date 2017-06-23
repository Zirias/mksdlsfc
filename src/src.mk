mksdlsfc_MODULES:= main convert config
mksdlsfc_posix_LIBS:= SDL2main SDL2 SDL2_image pocascore
mksdlsfc_win32_STATICLIBS:= mingw32 SDL2main SDL2_image SDL2 png z tiff jpeg lzma webp m dinput8 dxguid dxerr8 pocascore user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid

$(call binrules,mksdlsfc)

