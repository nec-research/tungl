__all__ = ['warn', 'error', 'debug', 'trace', 'info', 'log', 'throw', 'Level', '__version__', 'set_module', 'set_level']

import ctypes
from os import path
import enum
from typing import Tuple

__version__ = "@Tungl_VERSION@"

#-------------------------------------------------------------------------------
@enum.unique
class Level(enum.Enum):
	ERROR	= 1
	INFO	= 2
	WARN	= 3
	DEBUG	= 4
	TRACE	= 5

#-------------------------------------------------------------------------------
s_lib		= None
s_module	= 'Python'

#-------------------------------------------------------------------------------
def get_module():
	return s_module

#-------------------------------------------------------------------------------
def set_module(module: str):
	global s_module
	assert isinstance(module, str)
	s_module = module

#-------------------------------------------------------------------------------
def get_lib():
	global s_lib
	if s_lib is None:
		cwd = path.abspath(path.dirname(__file__))
		s_lib = ctypes.cdll.LoadLibrary(path.join(cwd, 'lib64/libtungl.so.@Tungl_VERSION_MAJOR@'))
		s_lib.tungl_log.argtypes		= (ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p)
		s_lib.tungl_log.restype			= None
		s_lib.tungl_throw.argtypes		= (ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p)
		s_lib.tungl_throw.restype		= None
		s_lib.tungl_set_level.argtypes	= (ctypes.c_int,)
		s_lib.tungl_set_level.restype	= None
	return s_lib

#-------------------------------------------------------------------------------
def set_level(level: Level):
	assert isinstance(level, Level)
	get_lib().tungl_set_level(level.value)
	
#-------------------------------------------------------------------------------
def log(level: Level, msg: str, file: str = '', line: int = 0) -> None:
	assert isinstance(level, Level)
	assert isinstance(msg, str)
	assert isinstance(file, str)
	assert isinstance(line, int)
	if len(msg) > 0:
		get_lib().tungl_log(level.value, s_module.encode('UTF-8'), file.encode('UTF-8'), line, msg.encode('UTF-8'))

#-------------------------------------------------------------------------------
def debug(*msg: Tuple[str], file: str = '', line: int = 0):	log(Level.DEBUG, ' '.join(msg), file, line)
def error(*msg: Tuple[str], file: str = '', line: int = 0):	log(Level.ERROR, ' '.join(msg), file, line)
def info (*msg: Tuple[str], file: str = '', line: int = 0):	log(Level.INFO,  ' '.join(msg), file, line)
def trace(*msg: Tuple[str], file: str = '', line: int = 0):	log(Level.TRACE, ' '.join(msg), file, line)
def warn (*msg: Tuple[str], file: str = '', line: int = 0):	log(Level.WARN,  ' '.join(msg), file, line)

#-------------------------------------------------------------------------------
def throw(msg: str, file: str = '', line: int = 0) -> None:
	assert isinstance(msg, str)
	assert isinstance(file, str)
	assert isinstance(line, int)
	get_lib().tungl_throw(s_module.encode('UTF-8'), file.encode('UTF-8'), line, msg.encode('UTF-8'))