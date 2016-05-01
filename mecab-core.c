/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <emacs-module.h>

#include <mecab.h>

int plugin_is_GPL_compatible;

static char*
retrieve_string(emacs_env *env, emacs_value str, ptrdiff_t *size)
{
	*size = 0;

	env->copy_string_contents(env, str, NULL, size);
	char *p = malloc(*size);
	if (p == NULL) {
		*size = 0;
		return NULL;
	}
	env->copy_string_contents(env, str, p, size);

	return p;
}

static void
el_mecab_free(void *arg)
{
	mecab_destroy((mecab_t*)arg);
}

static emacs_value
Fmecab_new(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value type = env->type_of(env, args[0]);
	mecab_t *mecab;

	if (!env->is_not_nil(env, args[0])) {
		mecab = mecab_new(0, NULL);
	} else if (env->eq(env, type, env->intern(env, "string"))) {
		ptrdiff_t size;
		char *dict = retrieve_string(env, args[0], &size);
		mecab = mecab_new2(dict);
		free(dict);
	} else if (env->eq(env, type, env->intern(env, "vector"))) {
		int argc = (int)env->vec_size(env, args[0]);
		char **argv = (char**)malloc(sizeof(char*) * argc);

		for (int i = 0; i < argc; ++i) {
			ptrdiff_t size;
			emacs_value dict = env->vec_get(env, args[0], i);
			argv[i] = retrieve_string(env, dict, &size);
		}

		mecab = mecab_new(argc, argv);

		for (int i = 0; i < argc; ++i) {
			free(argv[i]);
		}
		free(argv);
	} else {
		emacs_value errmsg = env->make_string(env, "Invalid argument",
						      sizeof("Invalid argument"));
		env->non_local_exit_signal(env, env->intern(env, "error"), errmsg);
		return env->intern(env, "nil");
	}

	return env->make_user_ptr(env, el_mecab_free, mecab);
}

static emacs_value
Fmecab_sparse_to_string(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	mecab_t *mecab = env->get_user_ptr(env, args[0]);
	ptrdiff_t size;
	char *input = retrieve_string(env, args[1], &size);
	int nbest = (int)env->extract_integer(env, args[2]);

	const char *ret;
	if (nbest < 0) {
		ret = mecab_sparse_tostr(mecab, input);
	} else {
		ret = mecab_nbest_sparse_tostr(mecab, nbest, input);
	}

	free(input);

	return env->make_string(env, ret, strlen(ret));
}

static emacs_value
Fmecab_sparse_to_list(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	mecab_t *mecab = env->get_user_ptr(env, args[0]);
	ptrdiff_t size;
	char *input = retrieve_string(env, args[1], &size);
	int limit = (int)env->extract_integer(env, args[2]);

	if (mecab_nbest_init(mecab, input) == 0) {
		free(input);
		emacs_value errmsg = env->make_string(env, "Failed mecab_nbest_init",
						      sizeof("Failed mecab_nbest_init"));
		env->non_local_exit_signal(env, env->intern(env, "error"), errmsg);
		return env->intern(env, "nil");
	}

	emacs_value ret = env->intern(env, "nil");
	emacs_value Qcons = env->intern(env, "cons");

	int n = 0;
	while (limit < 0 || n < limit) {
		const char *str = mecab_nbest_next_tostr(mecab);
		if (str == NULL)
			break;

		emacs_value estr = env->make_string(env, str, strlen(str));
		emacs_value args[] = {estr, ret};
		ret = env->funcall(env, Qcons, 2, args);

		++n;
	}

	free(input);

	return ret;
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("mecab-core-new", Fmecab_new, 1, 1, NULL, NULL);
	DEFUN("mecab-core-sparse-to-string", Fmecab_sparse_to_string, 3, 3, NULL, NULL);
	DEFUN("mecab-core-sparse-to-list", Fmecab_sparse_to_list, 3, 3, NULL, NULL);

#undef DEFUN

	provide(env, "mecab-core");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
