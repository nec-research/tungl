__all__ = [
	'warn',
	'error',
	'debug',
	'trace',
	'info',
	'throw',
	'Level',
	'__version__',
	'set_module',
	'set_level',
	'scope'
]

import ctypes
from os import path
import enum
from typing import Tuple, Any, Optional

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
s_file		= ''
s_module	= 'Python'

#-------------------------------------------------------------------------------
class Scope:
	def __init__(self, file: str, module: str):
		self._file		= file
		self._module	= module

	def _swap(self) -> None:
		global s_file, s_module
		s_file, self._file = self._file, s_file
		s_module, self._module = self._module, s_module

	def __enter__(self) -> None:
		self._swap()

	def __exit__(self, exc_type: Any, exc_value: Any, exc_traceback: Any) -> None:
		self._swap()

#-------------------------------------------------------------------------------
def scope(file: str=s_file, module: str=s_module) -> Scope:
	return Scope(file, module)

#-------------------------------------------------------------------------------
def get_module() -> str:
	return s_module

#-------------------------------------------------------------------------------
def set_module(module: str) -> None:
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
def set_level(level: Level) -> None:
	assert isinstance(level, Level)
	get_lib().tungl_set_level(level.value)
	
#-------------------------------------------------------------------------------
def join(msg: Tuple[str], sep: str) -> bytes:
	return sep.join(str(m) for m in msg).encode('UTF-8')

#-------------------------------------------------------------------------------
def log(*msg: Tuple[str], file: Optional[str] = None, line: int = 0, module: Optional[str] = None, sep: str = ' ', callback) -> None:
	if file is None:	file	= s_file
	if module is None:	module	= s_module
	assert isinstance(msg, tuple)
	assert isinstance(file, str)
	assert isinstance(line, int)
	assert isinstance(module, str)
	assert isinstance(sep, str)
	callback(module.encode('UTF-8'), file.encode('UTF-8'), line, join(msg, sep))

#-------------------------------------------------------------------------------
def debug(*args, **kwargs) -> None:	log(*args, callback=lambda *args: get_lib().tungl_log	(Level.DEBUG.value,	*args),	**kwargs)
def error(*args, **kwargs) -> None:	log(*args, callback=lambda *args: get_lib().tungl_log	(Level.ERROR.value,	*args),	**kwargs)
def info (*args, **kwargs) -> None:	log(*args, callback=lambda *args: get_lib().tungl_log	(Level.INFO.value,	*args),	**kwargs)
def trace(*args, **kwargs) -> None:	log(*args, callback=lambda *args: get_lib().tungl_log	(Level.TRACE.value,	*args),	**kwargs)
def warn (*args, **kwargs) -> None:	log(*args, callback=lambda *args: get_lib().tungl_log	(Level.WARN.value,	*args),	**kwargs)
def throw(*args, **kwargs) -> None: log(*args, callback=get_lib().tungl_throw, 											**kwargs)