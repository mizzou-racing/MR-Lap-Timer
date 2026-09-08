set(CMAKE_CXX_STANDARD 17)  # or 11, 14, etc., depending on your needs
set(CMAKE_CXX_STANDARD_REQUIRED ON)

idf_component_register(SRCS "helper.cpp" "main.cpp"
                    INCLUDE_DIRS "." "C:/esp/esp-idf/myProjects/LapTimer/managed_components/jgromes__RadioLib/examples/NonArduino/ESP-IDF/main")
