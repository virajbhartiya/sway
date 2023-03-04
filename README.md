# sway (Qualification task of CCExtractor for GSoC 2023)

## Changes made

Added the function `already_included()` which checks whether a given file path has already been included in the configuration file chain of a `sway_config` struct.

It does so by iterating through each file path in the config_chain array of the sway_config struct and comparing it to the provided path parameter using the strcmp function. If the path is found in the array, the function returns true, indicating that the file has already been included. If the path is not found in the array, the function returns false, indicating that the file has not yet been included.

```c
bool already_included(struct sway_config *config, const char *path) {
	for (int j = 0; j < config->config_chain->length; ++j) {
		if (strcmp(path, config->config_chain->items[j]) == 0) {
			return true;
		}
	}
	return false;
}
```

In the `load_include_configs()` function I've added the following code which iterates through each path in the array and if the path is include_one, it loads all files in the directory specified by the next path that have not already been included. If the path is not include_one, it loads the configuration file specified by the path.

The function keeps track of which files have already been included in the configuration by checking against a config struct pointer.

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

All the changes ave been made only in `sway/config.c` file.
