#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
typedef enum {
    FAILURE,
    SUCCESS
} Status;

typedef struct {
    char *input_filename;
    char *output_filename;
    int line_number_flag;
    FILE *src_fptr;
    FILE *dest_fptr;
    char *data;
    long filesize;
} Config;

/* Validation and file argument handling */
Status validate_file_extension(const char *filename);
Status parse_arguments(int argc, char *argv[], Config *cfg);

/* HTML formatting and boilerplate */
void write_html_header(FILE *dest, const char *title);
void write_html_footer(FILE *dest);
void write_escaped_char(char ch, FILE *dest);

/* Core token parser and classifier */
void convert_source_to_html(const char *src, FILE *dest, int enable_line_numbers);
Status is_keyword(const char *word);
Status is_single_operator(char ch);

#endif