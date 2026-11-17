# Source to HTML Converter

A command-line utility written in C that converts C source code (`.c`) into a syntax-highlighted HTML document (`.html`) for viewing in any web browser.

## Features

- **Syntax Highlighting:** Color-codes C keywords, preprocessor directives (including multi-line `#define` macros), string literals, character constants, numeric constants, operators, and single/multi-line comments.
- **Line Numbering:** Optional right-aligned line numbers enabled via CLI flags (`-n` or `-l`).
- **Custom Output File:** Specify a custom output HTML filename using `-o`, or automatically default to `<input_filename>.html`.
- **HTML Character Escaping:** Safely converts HTML-reserved characters (`<`, `>`, `&`, `"`) into HTML entities (`&lt;`, `&gt;`, `&amp;`, `&quot;`) so header includes and operators render properly.
- **Formatting Preservation:** Uses `<pre>` tags and handles both Unix (`\n`) and Windows (`\r\n`) line endings to maintain exact indentation and spacing.

## Project Structure

```text
├── main.c       # CLI argument parsing, file validation, and memory management
├── source.c     # Lexical scanner, token classification, and HTML generation
├── source.h     # Header file with data structures and function prototypes
└── test.c       # Sample C program for testing conversion
```

## Compilation

Compile the project using GCC:

```bash
gcc -Wall -Wextra main.c source.c -o source2html
```

## Usage

```bash
./source2html <source.c> [-o output.html] [-n]
```

### Examples

1. **Basic conversion** (generates `test.html`):
   ```bash
   ./source2html test.c
   ```

2. **Convert with line numbers enabled:**
   ```bash
   ./source2html test.c -n
   ```

3. **Specify a custom output file with line numbers:**
   ```bash
   ./source2html test.c -o highlighted.html -n
   ```

## Color Scheme

| C Token Category | Color Styling |
| :--- | :--- |
| Keywords (`int`, `for`, `return`, etc.) | Bold Blue (`#005cc5`) |
| Preprocessor Directives (`#include`, `#define`) | Bold Red (`#d73a49`) |
| String Literals (`"..."`) | Dark Navy (`#032f62`) |
| Numeric & Character Constants | Blue (`#005cc5`) |
| Operators (`+`, `=`, `>`, `&&`, etc.) | Red (`#d73a49`) |
| Comments (`//`, `/* ... */`) | Italic Gray (`#6a737d`) |
