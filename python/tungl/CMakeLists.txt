CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/tungl.py ${CMAKE_CURRENT_BINARY_DIR}/tungl.py @ONLY)

INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/__init__.py
	${CMAKE_CURRENT_BINARY_DIR}/tungl.py
DESTINATION
	${Tungl_INSTALL_PATH}
)