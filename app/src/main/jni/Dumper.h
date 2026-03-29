

#include "Utilidades/xdl.h"

bool dumper = true;

void Hook() {
	if (dumper) {
		il2cpp_dump();
		dumper = false;
		
	}
	void *il2cpp = xdl_open(targetLibName, XDL_DEFAULT);
	
	il2cpp_string_new = (MonoString *(*)(const char*))xdl_sym(il2cpp, "il2cpp_string_new", nullptr);
    
}

