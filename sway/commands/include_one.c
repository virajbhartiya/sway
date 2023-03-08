#include "sway/commands.h"
#include "sway/config.h"

struct cmd_results *cmd_include_one(int argc, char **argv) {
	struct cmd_results *error = NULL;

	if ((error = checkarg(argc, "include_one", EXPECTED_AT_LEAST, 2))) {
		return error;
	}

	const char **paths = (const char **) &argv[1];
	size_t num_paths = (size_t) (argc - 1);
	load_include_one_configs(paths, num_paths, config, &config->swaynag_config_errors);
	return cmd_results_new(CMD_SUCCESS, NULL);
}
