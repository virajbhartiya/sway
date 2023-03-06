# sway (Qualification task of CCExtractor for GSoC 2023)

## Changes made

I've created a new file called `sway/commands/include_one.c` which handels the functonality of `include_one` command

```c

struct cmd_results *cmd_include_one(int argc, char **argv) {
	struct cmd_results *error = NULL;

	if ((error = checkarg(argc, "include_one", EXPECTED_AT_LEAST, 1))) {
		return error;
	}


	load_include_configs(argv[0], config, &config->swaynag_config_errors);
	return cmd_results_new(CMD_SUCCESS, NULL);
}

```

Added the function `already_included()` checks whether a file with a given path has already been included in the configuration file chain of a `sway_config` struct. It does this by looping through each file path in the `config_chain` array of the `sway_config` struct and comparing it to the provided path parameter using the `strcmp` function and `strcmp(basename(path), basename(old_path)) == 0` checks for the indivisual file name. If the provided path or the file name is found in the array, the function returns true, indicating that the file has already been included. If the provided path is not found in the array, the function returns false, indicating that the file has not yet been included.

```c
bool already_included(struct sway_config *config, const char *path) {
    for (int j = 0; j < config->config_chain->length; ++j) {
        char *old_path = config->config_chain->items[j];
        if (strcmp(path, old_path) == 0 || strcmp(basename(path), basename(old_path)) == 0) {
            return true;
        }
    }
    return false;
}
```

In the `load_include_configs()` function, the code iterates through each path in the array and checks if it is equal to "include_one". If it is, the function loads all files in the directory specified by the next path that has not already been included. If the path is not "include_one", the function loads the configuration file specified by the path and also preserves the previous functionality of using just "include",

To keep track of which files have already been included in the configuration, the function uses a config struct pointer to check whether a file has already been loaded before.

```c
if (strcmp(w[i], "include_one") == 0) {
  if (i + 1 < p.we_wordc) {
    if (first_dir) {
      load_include_config(w[i+1], parent_dir, config, swaynag);
      first_dir = false;
    } else {
      DIR *dir = opendir(w[i+1]);
      if (dir == NULL) {
          sway_log(SWAY_ERROR, "Failed to open include_one directory");
          continue;
      }
      struct dirent *ent;
      while ((ent = readdir(dir)) != NULL) {
          if (ent->d_type == DT_REG) {
              char file_path[PATH_MAX];
              snprintf(file_path, sizeof(file_path), "%s/%s", w[i+1], ent->d_name);
              if (!already_included(config, file_path)) {
                  load_include_config(file_path, parent_dir, config, swaynag);
              }
          }
      }
      closedir(dir);
    }
    i++;
  } else {
      sway_log(SWAY_ERROR, "include_one missing argument");
  }
}

```

All the changes have been made only in [`sway/config.c`](https://github.com/virajbhartiya/sway/blob/master/sway/config.c) file.
