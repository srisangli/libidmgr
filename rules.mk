

%.o : %.c
	gcc -ggdb -Werror -c -fPIC -o $@ ${CFLAGS} -MMD -MF $(@:.o=.d) -MT'$@' $(filter %.c,$^) || rm -f $@
