include("C:/Users/Lenovo/Desktop/POC/CanTool/build/Desktop_Qt_6_10_2_MSVC2022_64bit-Release/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/CanTool-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/Users/Lenovo/Desktop/POC/CanTool/build/Desktop_Qt_6_10_2_MSVC2022_64bit-Release/CanTool.exe"
    GENERATE_QT_CONF
)
