# Tungl (Islandic for Moon) Logging Library

Tungl is a small portable logging library we wrote internally for some of our
projects.

## Release Notes
<table>
<tr><th>Version</th><th>Comment</th></tr>

<tr><td>v1.0.0</td><td>
Initial Release
</td></tr>

</table>

## How to use?
Tungl can be either be used by directly using the Tungl API

```c
#include <tungl.h>
tungl_log(TUNGL_LEVEL_INFO, "MyModule", __FILE__, __LINE__, "Message %i", variable);
```

or via the C and C++ macros.

```c
#include <tungl_macro.h> // C-style Macros

L_INFO("Message %i", variable);
```

```c++
#include <tungl_macro.hpp> //C++-style Macros
L_INFO("Message " << variable);
```