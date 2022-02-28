#include <tungl/macros.hpp>

int main(int argc, const char** argv) {
	tungl_set_level_str("TRACE");
	L_INFO("TEST123");
	L_WARN("TEST" << 123);
	L_TRACE("TEST" << 123 << " " <<  "with string");
	L_ERROR("TEST" << 123 << " " << "with string" << " " << argc);
	L_DEBUG("MUTLI " << 0 << std::endl << "LINE " << 1 << std::endl << "TEST 2" << std::endl);

	tungl_set_level(TUNGL_LEVEL_INFO);
	L_TRACE("INVISIBLE!");

	THROWIF(false, "Should not happen!");
	THROWIF(true, "ALL TESTS PASSED!");

	return 0;
}