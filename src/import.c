#include <string.h>
#include <stdio.h>
#include "interpreter.h"
#include "import.h"
#include "eval_types.h"

func_t *find_exported_func(eval_state_t *eval_state, module_t *module, char *func_name) {
    for (int i = 0; i < module->exports->length; i++) {
        if (strcmp(func_name, module->exports->values[i].name) == 0
            && module->exports->values[i].desc == EXPORTDESC_FUNC) {
            funcidx idx = module->exports->values[i].func;
            return &eval_state->module->funcs->values[idx];
        }
    }

    fprintf(stderr, "could not find function with name: %s in exports\n", func_name);
    interpreter_exit(eval_state);
}

global_t *find_exported_global(eval_state_t *eval_state, module_t *module, char *global_name) {
    for (int i = 0; i < module->exports->length; i++) {
        if (strcmp(global_name, module->exports->values[i].name) == 0
            && module->exports->values[i].desc == EXPORTDESC_GLOBAL) {
            globalidx idx = module->exports->values[i].global;
            return &eval_state->module->globals->values[idx];
        }
    }

    fprintf(stderr, "could not find global with name: %s in exports\n", global_name);
    interpreter_exit(eval_state);
}

func_t *find_func_global(eval_state_t *eval_state, char *module_name, char *func_name) {
    node_t *cur = eval_state->modules;

    for (int i = 0; cur != NULL; i++) {
        module_t *module = (module_t *) cur->data;

        if (strcmp(module_name, module->name) == 0) {
            return find_exported_func(eval_state, module, func_name);
        }

        cur = cur->next;
    }

    fprintf(stderr, "could not find module with name: %s\n", module_name);
    interpreter_exit(eval_state);
}

global_t *find_global_global(eval_state_t *eval_state, char *module_name, char *global_name) {
    node_t *cur = eval_state->modules;

    for (int i = 0; cur != NULL; i++) {
        module_t *module = (module_t *) cur->data;

        if (strcmp(module_name, module->name) == 0) {
            return find_exported_global(eval_state, module, global_name);
        }

        cur = cur->next;
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

    for (int i = 0; i < imports->length; i++) {
        import_t import = imports->values[i];

        if (import.desc == IMPORTDESC_FUNC) {
            eval_state->module->funcs->values[func_idx++] = *find_func_global(eval_state, import.module, import.name);
        } else if (import.desc == IMPORTDESC_GLOBAL) {
            eval_state->module->globals->values[global_idx++] = *find_global_global(eval_state, import.module,
                                                                                  import.name);
        } else if (import.desc == IMPORTDESC_TABLE) {
            //do nothing for now
        } else if (import.desc == IMPORTDESC_MEM) {
            //do nothing for now
        } else {
            interpreter_error(eval_state, "unknown import desc");
        }
    }
}
