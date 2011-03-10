project(tek)

cmake_minimum_required(VERSION 2.6)

set (VERSION x.xx)

# ==================================================
# Shared library for tek_user
# ==================================================

add_library(tek_user SHARED
	library/tek_user.cc library/tek_user.h
)
target_link_libraries(tek_user vxi11)

set_target_properties(tek_user PROPERTIES
	VERSION 0.${VERSION}
	SOVERSION 0
)

# ==================================================
# Command line utilities
# ==================================================

add_executable(tek_afg utils/tek_afg/tek_afg.cc)
target_link_libraries(tek_afg tek_user)

add_executable(tek_afg_upload_arb utils/tek_afg_upload_arb/tek_afg_upload_arb.cc)
target_link_libraries(tek_afg_upload_arb tek_user)

add_executable(tek_load_setup utils/tek_load_save_setup/tek_load_setup.cc)
target_link_libraries(tek_load_setup tek_user)

add_executable(tek_save_setup utils/tek_load_save_setup/tek_save_setup.cc)
target_link_libraries(tek_save_setup tek_user)

add_executable(tgetwf utils/tgetwf/tgetwf.cc)
target_link_libraries(tgetwf tek_user)

