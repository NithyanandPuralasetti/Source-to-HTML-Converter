#include "source.h"

const char *c_keywords[32] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

const char c_single_operators[16] = {
    '+', '-', '*', '/', '%', '=', '>', '<',
    '&', '|', '^', '~', '!', '?', ':', '.'
};

const char *c_double_operators[21] = {
    "++", "--", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", ">=", "<=", "&&", "||",
    "<<", ">>", "&=", "|=", "^=", "<<=", ">>=", "->"
};

/* Checks whether a token matches one of the 32 standard C keywords */
Status is_keyword(const char *word) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(word, c_keywords[i]) == 0) {
            return SUCCESS;
        }
    }
    return FAILURE;
}

/* Checks whether a character is a recognized single-character operator */
Status is_single_operator(char ch) {
    for (int i = 0; i < 16; i++) {
        if (ch == c_single_operators[i]) {
            return SUCCESS;
        }
    }
    return FAILURE;
}

/* Escapes HTML-reserved characters (<, >, &, ") before writing to output */
void write_escaped_char(char ch, FILE *dest) {
    switch (ch) {
        case '<': fputs("&lt;", dest); break;
        case '>': fputs("&gt;", dest); break;
        case '&': fputs("&amp;", dest); break;
        case '"': fputs("&quot;", dest); break;
        default:  fputc(ch, dest); break;
    }
}

/* Writes the opening HTML document tags and embedded CSS styling rules */
void write_html_header(FILE *dest, const char *title) {
    fprintf(dest, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(dest, "    <meta charset=\"UTF-8\">\n");
    fprintf(dest, "    <title>%s</title>\n", title);
    fprintf(dest, "    <style>\n");
    fprintf(dest, "        body { font-family: 'Consolas', 'Courier New', monospace; background-color: #fbfbfb; color: #24292e; }\n");
    fprintf(dest, "        pre { line-height: 1.5; font-size: 14px; margin: 0; padding: 12px; }\n");
    fprintf(dest, "        .keyword { color: #005cc5; font-weight: bold; }\n");
    fprintf(dest, "        .preprocessor { color: #d73a49; font-weight: bold; }\n");
    fprintf(dest, "        .string { color: #032f62; }\n");
    fprintf(dest, "        .char-const { color: #005cc5; }\n");
    fprintf(dest, "        .numeric { color: #005cc5; }\n");
    fprintf(dest, "        .comment { color: #6a737d; font-style: italic; }\n");
    fprintf(dest, "        .operator { color: #d73a49; }\n");
    fprintf(dest, "        .lineno { color: #babbbd; user-select: none; margin-right: 14px; text-align: right; display: inline-block; width: 40px; }\n");
    fprintf(dest, "    </style>\n</head>\n<body>\n<pre>");
}

/* Writes the closing HTML document tags */
void write_html_footer(FILE *dest) {
    fprintf(dest, "\n</pre>\n</body>\n</html>\n");
}

/* Tokenizes C source buffer and writes syntax-highlighted HTML to dest */
void convert_source_to_html(const char *src, FILE *dest, int enable_line_numbers) {
    int i = 0;
    int line = 1;

    if (enable_line_numbers) {
        fprintf(dest, "<span class=\"lineno\">%4d</span>", line);
    }

    while (src[i] != '\0') {
        if (src[i] == '\r') {
            i++;
            continue;
        }

        if (src[i] == '\n') {
            fputc('\n', dest);
            line++;
            i++;
            if (enable_line_numbers && src[i] != '\0') {
                fprintf(dest, "<span class=\"lineno\">%4d</span>", line);
            }
            continue;
        }

        /* Single-line comment */
        if (src[i] == '/' && src[i + 1] == '/') {
            fprintf(dest, "<span class=\"comment\">");
            while (src[i] != '\0' && src[i] != '\n' && src[i] != '\r') {
                write_escaped_char(src[i], dest);
                i++;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* Multi-line comment */
        if (src[i] == '/' && src[i + 1] == '*') {
            fprintf(dest, "<span class=\"comment\">/*");
            i += 2;
            while (src[i] != '\0' && !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\r') {
                    i++;
                } else if (src[i] == '\n') {
                    fputc('\n', dest);
                    line++;
                    if (enable_line_numbers) {
                        fprintf(dest, "<span class=\"lineno\">%4d</span>", line);
                    }
                    i++;
                } else {
                    write_escaped_char(src[i], dest);
                    i++;
                }
            }
            if (src[i] == '*' && src[i + 1] == '/') {
                fputs("*/", dest);
                i += 2;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* Preprocessor directive (supports multi-line macros ending with '\') */
        if (src[i] == '#') {
            fprintf(dest, "<span class=\"preprocessor\">");
            while (src[i] != '\0') {
                if (src[i] == '\r') {
                    i++;
                    continue;
                }
                if (src[i] == '\n') {
                    int prev = i - 1;
                    if (prev >= 0 && src[prev] == '\r') {
                        prev--;
                    }
                    if (prev >= 0 && src[prev] == '\\') {
                        fputc('\n', dest);
                        line++;
                        i++;
                        if (enable_line_numbers && src[i] != '\0') {
                            fprintf(dest, "<span class=\"lineno\">%4d</span>", line);
                        }
                        continue;
                    }
                    break;
                }
                write_escaped_char(src[i], dest);
                i++;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* String literal */
        if (src[i] == '"') {
            fprintf(dest, "<span class=\"string\">\"");
            i++;
            while (src[i] != '\0' && src[i] != '\n') {
                if (src[i] == '\\' && src[i + 1] != '\0') {
                    write_escaped_char(src[i], dest);
                    i++;
                    write_escaped_char(src[i], dest);
                    i++;
                    continue;
                }
                if (src[i] == '"') {
                    fputc('"', dest);
                    i++;
                    break;
                }
                write_escaped_char(src[i], dest);
                i++;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* Character constant */
        if (src[i] == '\'') {
            fprintf(dest, "<span class=\"char-const\">'");
            i++;
            while (src[i] != '\0' && src[i] != '\n') {
                if (src[i] == '\\' && src[i + 1] != '\0') {
                    write_escaped_char(src[i], dest);
                    i++;
                    write_escaped_char(src[i], dest);
                    i++;
                    continue;
                }
                if (src[i] == '\'') {
                    fputc('\'', dest);
                    i++;
                    break;
                }
                write_escaped_char(src[i], dest);
                i++;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* Numeric constant */
        if (isdigit((unsigned char)src[i]) || (src[i] == '.' && isdigit((unsigned char)src[i + 1]))) {
            fprintf(dest, "<span class=\"numeric\">");
            while (isalnum((unsigned char)src[i]) || src[i] == '.') {
                fputc(src[i], dest);
                i++;
            }
            fprintf(dest, "</span>");
            continue;
        }

        /* Keyword or identifier */
        if (isalpha((unsigned char)src[i]) || src[i] == '_') {
            char word[128];
            int w_idx = 0;
            while (isalnum((unsigned char)src[i]) || src[i] == '_') {
                if (w_idx < (int)sizeof(word) - 1) {
                    word[w_idx++] = src[i];
                }
                i++;
            }
            word[w_idx] = '\0';

            if (is_keyword(word)) {
                fprintf(dest, "<span class=\"keyword\">%s</span>", word);
            } else {
                fputs(word, dest);
            }
            continue;
        }

        /* Double-character operator */
        int matched_double = 0;
        for (int d = 0; d < 21; d++) {
            if (src[i] == c_double_operators[d][0] && src[i + 1] == c_double_operators[d][1]) {
                fprintf(dest, "<span class=\"operator\">");
                write_escaped_char(c_double_operators[d][0], dest);
                write_escaped_char(c_double_operators[d][1], dest);
                fprintf(dest, "</span>");
                i += 2;
                matched_double = 1;
                break;
            }
        }
        if (matched_double) {
            continue;
        }

        /* Single-character operator */
        if (is_single_operator(src[i])) {
            fprintf(dest, "<span class=\"operator\">");
            write_escaped_char(src[i], dest);
            fprintf(dest, "</span>");
            i++;
            continue;
        }

        /* Whitespace, punctuators, and remaining characters */
        write_escaped_char(src[i], dest);
        i++;
    }
}