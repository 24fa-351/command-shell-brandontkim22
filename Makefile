my_shell: my_shell.c exec_args.c exec_args.h path_funcs.c path_funcs.h handle_operations.c handle_operations.h
	gcc -o my_shell my_shell.c exec_args.c path_funcs.c handle_operations.c

clean:
	rm -f my_shell