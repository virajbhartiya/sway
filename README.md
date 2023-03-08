# sway (Qualification task of CCExtractor for GSoC 2023)

## Changes made

I've created a new file called `sway/commands/include_one.c` which handels the functonality of `include_one` command

```c

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

```

Added the function `already_included()`, `extract_file_name()` and `same_filenme()` which check whether a file with a given path has already been included in the configuration file chain of a `sway_config` struct. It does this by looping through each file path in the `config_chain` array of the `sway_config` struct and comparing it to the provided path parameter using the `strcmp` function and `same_filename(path, old_path)` checks for the indivisual file name. If the provided path or the file name is found in the array, the function returns true, indicating that the file has already been included. If the provided path is not found in the array, the function returns false, indicating that the file has not yet been included.

```c
bool already_included(struct sway_config *config, const char *path) {
    for (int j = 0; j < config->config_chain->length; ++j) {
        char *old_path = config->config_chain->items[j];
        if (strcmp(path, old_path) == 0 ||  same_filename(path, old_path)) {
            return true;
        }
    }
    return false;
}

char *extract_filename(const char *path) {
    const char *last_sep = strrchr(path, '/');
    if (last_sep != NULL) {
        return (char *)(last_sep + 1);
    }
    return (char *)path;
}

// Checks if two paths have the same filename.
bool same_filename(const char *path1, const char *path2) {
    const char *filename1 = extract_filename(path1);
    const char *filename2 = extract_filename(path2);
    return (strcmp(filename1, filename2) == 0);
}
```

Created a new `load_include_one_configs()` function, the code iterates through each path in the array and checks if it is equal to "include_one". If it is, the function loads all files in the directory specified by the next path that has not already been included. If the path is not "include_one", the function loads the configuration file specified by the path and also preserves the previous functionality of using just "include",

To keep track of which files have already been included in the configuration, the function uses a config struct pointer to check whether a file has already been loaded before.

```c
void load_include_one_configs(const char **paths, size_t num_paths, struct sway_config *config,
		struct swaynag_instance *swaynag) {
	char *wd = getcwd(NULL, 0);
	char *parent_path = strdup(config->current_config_path);
	const char *parent_dir = dirname(parent_path);

	if (chdir(parent_dir) < 0) {
		sway_log(SWAY_ERROR, "failed to change working directory");
		goto cleanup;
	}

	for (size_t i = 0; i < num_paths; ++i) {
		wordexp_t p;
		if (wordexp(paths[i], &p, 0) == 0) {
			char **w = p.we_wordv;
			size_t j;
			bool first_dir = true;
			for (j = 0; j < p.we_wordc; ++j) {
				if (j <= p.we_wordc) {
					if (first_dir) {
						load_include_config(w[j+1], parent_dir, config, swaynag);
						first_dir = false;
					} else {
						// load only files not already included
						DIR *dir = opendir(w[j+1]);
						if (dir == NULL) {
							sway_log(SWAY_ERROR, "Failed to open include_one directory");
							continue;
						}
						struct dirent *ent;
						while ((ent = readdir(dir)) != NULL) {
							if (S_ISREG(ent->d_type)) {
								char file_path[PATH_MAX];
								snprintf(file_path, sizeof(file_path), "%s/%s", w[j+1], ent->d_name);
								if (!already_included(config, file_path)) {
									load_include_config(file_path, parent_dir, config, swaynag);
								}
							}
						}
						closedir(dir);
					}
					j++;
				} else {
					sway_log(SWAY_ERROR, "include_one missing argument");
				}
			}
			wordfree(&p);
		}
	}

	// Attempt to restore working directory before returning.
	if (chdir(wd) < 0) {
		sway_log(SWAY_ERROR, "failed to change working directory");
	}
cleanup:
	free(parent_path);
	free(wd);
}


```
