# Setting up app build
set(APP_NAME ${CMAKE_PROJECT_NAME})

# Collect all files from source dir inside SOURCES_FILES and check if they were updated
file(GLOB_RECURSE SOURCES_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c)

add_executable(${APP_NAME}
    ${SOURCES_FILES}
)

target_include_directories(${APP_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_link_libraries(${APP_NAME}
    PRIVATE
        mik32_hal
)

target_link_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/hardware/mik32v2-shared/ldscripts
        .
)

# Convert output to hex and binary
add_custom_command(TARGET ${APP_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O binary ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.bin
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O ihex ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.hex
    COMMAND ${TOOLCHAIN_PREFIX}-objdump -w -x -s -S ${APP_NAME}.elf > ${APP_NAME}.dump
)
