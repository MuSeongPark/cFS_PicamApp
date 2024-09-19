cmake_minimum_required(VERSION 2.6.4)
project(CFE_PICAM_APP C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

include_directories(${serial_lib_MISSION_DIR}/fsw/public_inc) #added

aux_source_directory(fsw/src APP_SRC_FILES)

add_cfe_app(picam ${APP_SRC_FILES})

#Adding table is required
add_cfe_tables(PicamAppTable fsw/tables/picam_app_tbl.c)