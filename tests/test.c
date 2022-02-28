#include <tungl/macros.h>

int main(int argc, const char** argv) {
	tungl_set_level_str("TRACE");
	L_INFO("TEST123");
	L_WARN("TEST%i", 123);
	L_TRACE("TEST%i %s", 123, "with string");
	L_ERROR("TEST%i %s %p", 123, "with string", argc);
	L_DEBUG("MUTLI %i\nLINE %i\nTEST %i\n", 0, 1, 2);

	tungl_set_level(TUNGL_LEVEL_INFO);
	L_TRACE("INVISIBLE!");

	THROWIF(0, "Should not happen!");
	THROWIF(1, "ALL TESTS PASSED!");	

	return 0;
}