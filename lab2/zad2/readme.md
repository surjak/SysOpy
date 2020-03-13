## FIND

## How to use?

void print_files_handler(char *root_path, char *comm, char mode, int count, int maxdep);

* root_path - path to directory
* comm - "maxdepth" or "atime" or "mtime"
* mode - "-" or "+" or "="
* count - number after mode
* maxdep - maxdepth (-1 - if you don't want)

## Examples

```c
print_files_handler("../../", "mtime", '+', 3, 2);
```

The command is similar to: 

```bash
find -maxdepth 2 -mtime +3
```