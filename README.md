# Tungl (Islandic for Moon) Logging Library

Tungl is a small portable logging library we wrote internally for some of our
projects.

## Release Notes
<table>
<tr><th>Version</th><th>Comment</th></tr>

<tr><td>v0.1.2</td><td>
Lowered requirements to be Python3.6 compatible.
</td></tr>

<tr><td>v0.1.1</td><td>
Added <code>TUNGL_TIME</code> env var.
</td></tr>

<tr><td>v0.1.0</td><td>
Initial Release
</td></tr>

</table>

## How to use?
Tungl can be either be used by directly using the Tungl API:

```c
#include <tungl/api.h>
tungl_log(TUNGL_LEVEL_INFO, "MyModule", __FILE__, __LINE__, "Message %i", variable);
```

or via the C and C++ macros.

```c
#define L_MODULE "MyModule"
#include <tungl/c.h> // C-style Macros
L_INFO("Message %i", variable);
```

```c++
#define L_MODULE "MyModule"
#include <tungl/cxx.h> // C++-style Macros
L_INFO("Message " << variable);
```

Available macros (in both API's are):
```c++
#define L_STR(...)                       // transforms L_STR(abc) to "abc"
#define L_INFO(...)                      // prints INFO message
#define L_ERROR(...)                     // prints ERROR message
#define L_WARN(...)                      // prints WARN message
#define L_DEBUG(...)                     // prints DEBUG message
#define L_TRACE(...)                     // prints TRACE message
#define THROWAT(MODULE, FILE, LINE, ...) // throws ERROR message and kills the application, reporting FILE and LINE
#define THROW(...)                       // throws ERROR message and kills the application
#define THROWIF(COND, ...)               // throws ERROR message if COND is fulfilled
#define ASSERT(COND)                     // checks COND and prints COND if not successful
#define FAIL()                           // throws "Implementation Error"
#define TODO(...)                        // throws TODO message
```

or in python!
```python
import tungl
tungl.set_module('my awesome python application')
tungl.get_module()
tungl.set_level(tungl.Level.[ERROR, INFO, WARN, DEBUG, TRACE])
tungl.info ("Message!", file=__file__)
tungl.trace("Message!", file=__file__)
tungl.error("Message!", file=__file__)
tungl.warn ("Message!", file=__file__)
tungl.debug("Message!", file=__file__)
tungl.throw("Message!", file=__file__)
# there is no convenient way to automatically determine the line in Python,
# BUT you can set it manually if you like.
```

## Advanced features
```TUNGL_LOG=[INFO, WARN, ERROR, DEBUG, TRACE]```,
```tungl_set_level(tungl_log_level_t)``` or ```tungl_set_level_str(const
char*)``` can be used to set the log level.

Tungl also supports to write a copy of the log directly to a file using
```TUNGL_FILE=/any/valid/file/name``` or ```tungl_set_file(const char*)```. All
colors will be stripped before writing to the file.

```TUNGL_COLOR=OFF``` can be used to disable color output.

## How to build?

```bash
python3 -m install illyrian

git clone ...

mkdir tungl/build
cd tungl/build

# Option 1: Local installation (default CMAKE_INSTALL_PREFIX=/usr/local/ve)
illyrian cmake -DTungl_DIST_TYPE=LOCAL ..
cmake --build . --target install

# Option 2: Python package
illyrian cmake -DTungl_DIST_TYPE=PYTHON ..
cmake --build . --target dist
```

