cmake_minimum_required(VERSION 3.19)
project(CanTool LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets PrintSupport)

qt_standard_project_setup()

set(VECTOR_BLF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/logging/include/")
set(VECTOR_BLF_LIB_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/logging/lib/")

if(NOT EXISTS "${VECTOR_BLF_INCLUDE_DIR}/binlog.h")
    message(FATAL_ERROR "Not found binlog.h in ${VECTOR_BLF_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${VECTOR_BLF_LIB_DIR}/binlog.lib")
    message(WARNING "Not found binlog.lib in ${VECTOR_BLF_LIB_DIR}")
endif()


qt_add_executable(CanTool
    WIN32 MACOSX_BUNDLE
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui

    app/CanController.h app/CanController.cpp
    can/raw/CanRawFrame.h
    can/receive/CanReceiver.h can/receive/CanReceiver.cpp
    can/dbc/DbcManager.h
    can/dbc/DbcManager.cpp
    can/model/CanInformation.h
    can/model/CanInformation.cpp
    can/dbc/DbcLibWrapper.h
    can/model/CanMessageInfo.h
    can/model/CanSignalInfo.h
    can/driver/ICanDriver.h
    can/driver/VectorCanDriver.h
    can/driver/VectorCanDriver.cpp
    can/runtime/SignalSample.h
    can/runtime/SignalBuffer.h
    can/runtime/SignalStore.h
    can/runtime/SignalBuffer.cpp
    can/runtime/SignalBuffer.cpp
    can/runtime/SignalBuffer.cpp
    can/runtime/SignalStore.cpp
    can/decode/DecodedSignalValue.h
    can/metadata/SignalMeta.h
    can/metadata/SignalClassifier.h
    can/metadata/SignalClassifier.cpp
    uicontroller/PlotController.h
    uicontroller/PlotController.cpp
    ui/plot/SignalPlotWidget.h
    ui/plot/TimeSignalPlot.h
    ui/plot/TimeSignalPlot.cpp
    ui/plot/DigitalSignalPlot.h
    ui/plot/DigitalSignalPlot.cpp
    ui/plot/XYPlotWidget.h
    ui/plot/XYPlotWidget.cpp
    utils/TimeUtils.h

# qcustomplot
    thirdparty/qcustomplot/qcustomplot.h
    thirdparty/qcustomplot/qcustomplot.cpp
# XlDriver
    thirdparty/vector/vxlapi.h
#screen
    ui/graphicswindow/graphicwidget.h
    ui/graphicswindow/graphicwidget.cpp
    ui/graphicswindow/graphicwidget.ui

    ui/tracewindow/tracewidget.h
    ui/tracewindow/tracewidget.cpp
    ui/tracewindow/tracewidget.ui
#search signal
    can/model/CanSignalSearch.h
    can/model/CanSignalSearch.cpp
    can/filter/CanSignalFilterModel.h
    can/filter/CanSignalFilterModel.cpp
#write .blf log
    can/log/ICanLogWriter.h
    can/log/CanLogWriter.h can/log/CanLogWriter.cpp
    can/log/BlfFileManager.h can/log/BlfFileManager.cpp
)


target_include_directories(CanTool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/can
    ${CMAKE_CURRENT_SOURCE_DIR}/can/dbc
    ${CMAKE_CURRENT_SOURCE_DIR}/can/model
    ${CMAKE_CURRENT_SOURCE_DIR}/can/raw
    ${CMAKE_CURRENT_SOURCE_DIR}/can/receive
    ${CMAKE_CURRENT_SOURCE_DIR}/can/decode
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_SOURCE_DIR}/thirdparty/dbclib/include
    ${CMAKE_SOURCE_DIR}/thirdparty/qcustomplot
    ${CMAKE_SOURCE_DIR}/thirdparty/vector
    ${VECTOR_BLF_INCLUDE_DIR}
)

#dbc lib
add_library(dbclib STATIC IMPORTED)

set_target_properties(dbclib PROPERTIES
    IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dbclib/lib/dbc.lib
    INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dbclib/include
)
#xldriver
add_library(vxlapi STATIC IMPORTED)

set_target_properties(vxlapi PROPERTIES
    IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vector/vxlapi64.lib
    INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_SOURCE_DIR}/thirdparty/vector
)

target_link_libraries(CanTool
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::PrintSupport
        dbclib
        vxlapi
        "$<$<PLATFORM_ID:Windows>:${VECTOR_BLF_LIB_DIR}/binlog.lib>"
)

if(WIN32)
    add_custom_command(TARGET CanTool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VECTOR_BLF_LIB_DIR}/binlog.dll"
            $<TARGET_FILE_DIR:CanTool>
        COMMENT "Copy binlog.dll to executable"
    )
endif()

include(GNUInstallDirs)

install(TARGETS CanTool
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CanTool
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
