#include <string.h>
#include <stdio.h>
#include "interpreter.h"
#include "import.h"
#include "eval_types.h"

func_t *find_exported_func(eval_state_t *eval_state, module_t *module, char *func_name) {
    for (int i = 0; i < vec_export_length(module->exports); i++) {
        export_t *export = vec_export_getp(module->exports, i);
        if (strcmp(func_name, export->name) == 0 && export->desc == EXPORTDESC_FUNC) {
            funcidx idx = export->func;
            return vec_func_getp(eval_state->module->funcs, idx);
        }
    }

    fprintf(stderr, "could not find function with name: %s in exports\n", func_name);
    interpreter_exit(eval_state);
}

global_t *find_exported_global(eval_state_t *eval_state, module_t *module, char *global_name) {
    for (int i = 0; i < vec_export_length(module->exports); i++) {
        export_t *export = vec_export_getp(module->exports, i);
        if (strcmp(global_name, export->name) == 0 && export->desc == EXPORTDESC_GLOBAL) {
            globalidx idx = export->global;
            return vec_global_getp(eval_state->module->globals, idx);
        }
    }

    fprintf(stderr, "could not find global with name: %s in exports\n", global_name);
    interpreter_exit(eval_state);
}

func_t *find_func_global(eval_state_t *eval_state, char *module_name, char *func_name) {
    vec_module_iterator_t it = vec_module_iterator(eval_state->modules, IT_FORWARDS);
    while (vec_module_has_next(&it)) {
        module_t *module = vec_module_nextp(&it);
        if (strcmp(module_name, module->name) == 0) {
            return find_exported_func(eval_state, module, func_name);
        }
    }

    fprintf(stderr, "could not find module with name: %s\n", module_name);
    interpreter_exit(eval_state);
}

global_t *find_global_global(eval_state_t *eval_state, char *module_name, char *global_name) {
    vec_module_iterator_t it = vec_module_iterator(eval_state->modules, IT_FORWARDS);
    while (vec_module_has_next(&it)) {
        module_t *module = vec_module_nextp(&it);
        if (strcmp(module_name, module->name) == 0) {
            return find_exported_global(eval_state, module, global_name);
        }
    }

    fprintf(stderr, "could not find module with name: %s\n", module_name);
    interpreter_exit(eval_state);
}

void init_imports(eval_state_t *eval_state, vec_import_t *imports) {
    if (imports == NULL) {
        return;
    }

    uint32_t func_idx = 0;
    uint32_t global_idx = 0;

    // TODO: fix
//    for (int i = 0; i < imports->length; i++) {
//        import_t import = imports->values[i];
//
//        if (import.desc == IMPORTDESC_FUNC) {
//            eval_state->module->funcs->values[func_idx++] = *find_func_global(eval_state, import.module, import.name);
//        } else if (import.desc == IMPORTDESC_GLOBAL) {
//            eval_state->module->globals->values[global_idx++] = *find_global_global(eval_state, import.module,
//                                                                                    import.name);
//        } else if (import.desc == IMPORTDESC_TABLE) {
//            //do nothing for now
//        } else if (import.desc == IMPORTDESC_MEM) {
//            //do nothing for now
//        } else {
//            interpreter_error(eval_state, "unknown import desc");
//        }
//    }
}
