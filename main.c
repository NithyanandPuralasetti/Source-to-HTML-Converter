#include "source.h"

/* Checks if the given filename ends with a .c extension */
Status validate_file_extension(const char *filename) {
    int len = strlen(filename);
    if (len >= 2 && strcmp(filename + len - 2, ".c") == 0) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Parses CLI flags (-o, -n/-l) and sets default output filename if omitted */
Status parse_arguments(int argc, char *argv[], Config *cfg) {
    cfg->input_filename = NULL;
    cfg->output_filename = NULL;
    cfg->line_number_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "-l") == 0) {
            cfg->line_number_flag = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                cfg->output_filename = argv[++i];
            } else {
                printf("Error: Missing argument following -o\n");
                return FAILURE;
            }
        } else if (argv[i][0] != '-') {
            if (cfg->input_filename == NULL) {
                cfg->input_filename = argv[i];
            }
        } else {
            printf("Error: Unknown flag '%s'\n", argv[i]);
            return FAILURE;
        }
    }

    if (cfg->input_filename == NULL) {
        printf("Usage: %s <source.c> [-o output.html] [-n]\n", argv[0]);
        return FAILURE;
    }

    if (!validate_file_extension(cfg->input_filename)) {
        printf("Error: Input file '%s' must have a .c extension\n", cfg->input_filename);
        return FAILURE;
    }

    /* Default output filename to <name>.html if -o is omitted */
    if (cfg->output_filename == NULL) {
        static char default_out[256];
        const char *ext = strrchr(cfg->input_filename, '.');
        if (ext != NULL) {
            int base_len = (int)(ext - cfg->input_filename);
            if (base_len > (int)sizeof(default_out) - 6) {
                base_len = (int)sizeof(default_out) - 6;
            }
            snprintf(default_out, sizeof(default_out), "%.*s.html", base_len, cfg->input_filename);
        } else {
            snprintf(default_out, sizeof(default_out), "%.*s.html", (int)sizeof(default_out) - 6, cfg->input_filename);
        }
        cfg->output_filename = default_out;
    }

    return SUCCESS;
}

/* Entry point: reads source C file and generates syntax-highlighted HTML output */
int main(int argc, char *argv[]) {
    Config cfg;

    if (parse_arguments(argc, argv, &cfg) == FAILURE) {
        return 1;
    }

    cfg.src_fptr = fopen(cfg.input_filename, "r");
    if (cfg.src_fptr == NULL) {
        printf("Error: Unable to open input file '%s'\n", cfg.input_filename);
        return 1;
    }

    cfg.dest_fptr = fopen(cfg.output_filename, "w");
    if (cfg.dest_fptr == NULL) {
        printf("Error: Unable to create output file '%s'\n", cfg.output_filename);
        fclose(cfg.src_fptr);
        return 1;
    }

    /* Read full source file into memory */
    fseek(cfg.src_fptr, 0, SEEK_END);
    cfg.filesize = ftell(cfg.src_fptr);
    rewind(cfg.src_fptr);

    cfg.data = (char *)malloc(cfg.filesize + 1);
    if (cfg.data == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(cfg.src_fptr);
        fclose(cfg.dest_fptr);
        return 1;
    }

    size_t bytes_read = fread(cfg.data, 1, cfg.filesize, cfg.src_fptr);
    cfg.data[bytes_read] = '\0';
    fclose(cfg.src_fptr);

    /* Build the HTML document */
    write_html_header(cfg.dest_fptr, cfg.input_filename);
    convert_source_to_html(cfg.data, cfg.dest_fptr, cfg.line_number_flag);
    write_html_footer(cfg.dest_fptr);

    fclose(cfg.dest_fptr);
    free(cfg.data);

    printf("Successfully converted '%s' to '%s'\n", cfg.input_filename, cfg.output_filename);
    return 0;
}