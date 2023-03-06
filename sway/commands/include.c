#include "sway/commands.h"
#include "sway/config.h"

struct cmd_results *cmd_include(int argc, char **argv) {
	struct cmd_results *error = NULL;
	int expected_argc;
	char *expected_arg;

	if (strcmp(cmd_name, "include") == 0) {
		expected_argc = EXPECTED_EQUAL_TO;
		expected_arg = "include";
	} else if (strcmp(cmd_name, "include_one") == 0) {
		expected_argc = EXPECTED_AT_LEAST;
		expected_arg = "include_one";
	} 

	if ((error = checkarg(argc, expected_arg, expected_argc, 1))) {
		return error;
	}

	load_include_configs(argv[0], config, &config->swaynag_config_errors);
	return cmd_results_new(CMD_SUCCESS, NULL);
}
