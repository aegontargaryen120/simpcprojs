#define ZAP_IMPLEMENTATION
#include "./zap.h"

#include <dirent.h>

void usage(FILE *stream) {
#define print(...) fprintf(stream, __VA_ARGS__)

  print("Usage: ./zap ([SUBCOMMAND] | [OPTIONS])\n");
  print("Subcommands:\n");
  print("    build <project>    Build a project.\n");
  print("    build all          Build all projects.\n");
  print("Options:\n");
  print("    -h, --help         Display this help message.\n");

#undef print
}

bool build(const char *project) {
  char input[PATH_MAX];
  char output[PATH_MAX];

  if (!mkdir_if_not_exists("bin")) {
    fprintf(stderr, err_fmt"Directory creation failed.\n");
    return false;
  }
  
  snprintf(input, sizeof(input), "%s.c", project);
  snprintf(output, sizeof(output), "bin/%s", project);

  if (!file_exists(input)) {
    fprintf(stderr, err_fmt"Source file '%s' does not exist.\n", input);
    return false;
  }

  Cmd cmd = {0};

  cc(&cmd);
  cc_flags(&cmd);
  cc_output(&cmd, output);
  cc_inputs(&cmd, input);

  printf(cmd_fmt);
  cmd_dump(&cmd);

  bool success = cmd_run(&cmd);
  cmd_free(&cmd);

  if (!success) {
    fprintf(stderr, err_fmt"Failed to build '%s'.\n", project);
    return false;
  }

  fprintf(stderr, info_fmt"Built '%s'.\n", project);

  return true;
}

bool build_all(void) {
  DIR *dir = opendir(".");

  if (dir == NULL) {
    perror("opendir");
    return false;
  }

  struct dirent *entry;
  bool success = true;

  while ((entry = readdir(dir)) != NULL) {
    size_t len = strlen(entry->d_name);

    if (len <= 2 ||
	strcmp(entry->d_name + len - 2, ".c") != 0)
      continue;

    if (strcmp(entry->d_name, "zap.c") == 0)
      continue;

    char project[PATH_MAX];

    snprintf(project, sizeof(project), "%.*s",
	     (int)(len - 2), entry->d_name);

    if (!build(project))
      success = false;
  }

  closedir(dir);

  return success;
}

int main(int argc, char **argv) {
  if (!ZAP_REBUILD_URSELF(argc, argv)) {
    fprintf(stderr, err_fmt"Could not rebuild file.\n");
    return 1;
  }

  if (argc < 2 ||
      strcmp(argv[1], "--help") == 0 ||
      strcmp(argv[1], "-h") == 0) {
    usage(stdout);
    return 0;
  }

  if (strcmp(argv[1], "build") == 0) {
    if (argc < 3) {
      fprintf(stderr, err_fmt"Missing project name.\n");
      usage(stderr);
      return 1;
    }

    if (strcmp(argv[2], "all") == 0)
      return build_all() ? 0 : 1;

    return build(argv[2]) ? 0 : 1;
  }

  fprintf(stderr, err_fmt"Unknown subcommand '%s'.\n", argv[1]);
  usage(stderr);

  return 1;
}
