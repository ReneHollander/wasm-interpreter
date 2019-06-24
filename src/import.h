#ifndef WASM_INTERPRETER_IMPORT_H
#define WASM_INTERPRETER_IMPORT_H

void init_imports(eval_state_t *eval_state, vec_import_t *imports);

func_t *find_exported_func(eval_state_t *eval_state, module_t *module, char *func_name);

#endif //WASM_INTERPRETER_IMPORT_H
