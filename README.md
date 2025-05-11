JSON to Relational CSV Converter
This project implements a JSON-to-Relational-CSV converter using Flex and Bison. It parses JSON input, constructs an Abstract Syntax Tree (AST), and generates relational CSV files based on the JSON structure. The program supports robust error handling for lexical, syntax, and file-related errors, and includes options for printing the AST or specifying an output directory.
Authors:

Tauha Imran (22i-1239)
Hussain Ali (22i-0902)

Build Instructions
Prerequisites

Operating System: Linux (tested on Ubuntu)
Tools:
GCC (gcc)
Bison (bison)
Flex (flex)
Make (make)


Libraries: Flex library (libfl-dev)

Install dependencies on Ubuntu:
sudo apt-get update
sudo apt-get install gcc bison flex libfl-dev make

Project Structure

src/: Source code directory
parser.y: Bison grammar for JSON parsing
scanner.l: Flex lexer for JSON tokens
main.c: Program entry point and argument handling
ast.h, ast.c: AST node definitions and manipulation
csv.h, csv.c: CSV generation and file handling
Makefile: Build script


Test files (e.g., test1.json, test_error2.json, test_error3.json)

Build Steps

Navigate to the source directory:cd ~/cc-assignment/4/vb/JSON-to-Relational-CSV-with-Flex---Yacc/src


Clean previous build artifacts (optional):make clean


Build the executable:make

This generates the json2relcsv executable.

Troubleshooting Build Issues

Missing Tools: Ensure gcc, bison, flex, and make are installed (bison --version, flex --version).
Linker Errors: Verify libfl-dev is installed (sudo apt-get install libfl-dev).
Multiple Definition Errors: Ensure global variables (e.g., root) are defined only once (in main.c) and declared as extern elsewhere.

Run Instructions
Usage
./json2relcsv [--print-ast] [--out-dir <directory>] < <input.json>


Options:
--print-ast: Print the AST instead of generating CSV files.
--out-dir <directory>: Specify the output directory for CSV files (default: current directory).


Input: JSON data is read from standard input (stdin).

Examples

Valid JSON Input:
echo '{
  "id": 1,
  "name": "Ali",
  "age": 19
}' > test1.json
./json2relcsv < test1.json

Output: Generates people.csv:
id,name,age
1,Ali,19


Lexical Error (Unterminated String):
echo '{
  "id": 1
  "name": "Ali
}' > test_error2.json
./json2relcsv < test_error2.json

Output:
Error: Unterminated string at line 3, column 11: "Ali

Exit status: 1

Syntax Error (Missing Comma):
echo '{
  "id": 1
  "name": "Ali"
}' > test_error3.json
./json2relcsv < test_error3.json

Output:
Error: Missing comma in object at line 3, column 5: "name"


File Error (Invalid Output Directory):
./json2relcsv --out-dir /invalid/dir < test1.json

Output:
Error: Cannot open file: /invalid/dir/root.csv


Print AST:
./json2relcsv --print-ast < test1.json

Output: Prints the AST structure (format depends on print_ast implementation in ast.c).


Testing
Test files are provided to verify functionality:

test1.json: Valid JSON input.
test_error2.json: Unterminated string error.
test_error3.json: Missing comma error.

Run tests:
./json2relcsv < test1.json
./json2relcsv < test_error2.json
./json2relcsv < test_error3.json
./json2relcsv --out-dir /invalid/dir < test1.json
./json2relcsv --print-ast < test1.json

Design Notes
Overview
The program converts JSON input into relational CSV files by:

Lexing: Tokenizing JSON input using Flex.
Parsing: Building an AST using Bison.
Processing: Traversing the AST to generate relational CSV files.
Error Handling: Reporting lexical, syntax, and file errors with precise line/column information.

Key Components

Flex Lexer (scanner.l):

Reentrant: Uses %option reentrant, %option bison-bridge, and %option bison-locations for thread-safe scanning and location tracking.
Tokens: Recognizes JSON tokens ({, }, [, ], :, ,, strings, numbers, true, false, null).
Error Handling:
Unterminated strings (e.g., "Ali).
Invalid escape sequences (e.g., \x).
Invalid characters.
Errors are reported with line/column numbers and context (via report_error).


String Processing: The unescape_string function handles JSON string escapes (e.g., \", \n, \uXXXX).


Bison Parser (parser.y):

Reentrant: Uses %define api.pure full, %locations, %lex-param, and %parse-param for a pure parser with scanner state.
Grammar: Defines JSON grammar (objects, arrays, strings, numbers, booleans, null).
AST Construction: Builds an AST using ast.h functions (e.g., create_object_node, create_pair_node).
Error Handling:
Syntax errors (e.g., missing commas, missing colons) are caught via error productions (e.g., pair: STRING error).
Errors are reported with line/column numbers and token context (via yyerror and report_error).


Global Variables: Uses extern struct ast_node *root to store the AST root, defined in main.c.


AST Processing (ast.h, ast.c):

Node Types: Defines AST nodes for objects, arrays, pairs, strings, numbers, booleans, and null values.
Functions:
Creation: create_string_node, create_number_node, etc.
Linking: append_pair, append_element for building object/array lists.
Printing: print_ast for debugging.


Memory Management: Nodes are dynamically allocated and freed via cleanup.


CSV Generation (csv.h, csv.c):

Processing: The process_ast function traverses the AST to generate relational CSV files.
File Handling: Writes CSV files to the specified output directory (or current directory if unspecified).
Error Handling: Reports file access errors (e.g., invalid directory) via report_error.
Output Format: Generates CSV files with headers based on JSON keys (e.g., id,name,age).


Main Program (main.c):

Initialization: Sets up the reentrant scanner (yylex_init) and global variables (root, yytext, line, column).
Arguments:
--print-ast: Outputs the AST.
--out-dir <directory>: Specifies the CSV output directory.


Parsing: Calls yyparse to build the AST.
Processing: Invokes print_ast or process_ast based on arguments.
Cleanup: Frees resources via cleanup and yylex_destroy.



Design Decisions

Reentrant Parser:

Chosen for thread-safety and flexibility, using Flex/Bison reentrant options.
Location tracking (%locations) provides precise error reporting (line/column numbers).
Challenges: Required careful management of yyscan_t, YYSTYPE, and YYLTYPE, resolved by proper header inclusions and declarations.


Error Handling:

Lexical Errors: Handled in scanner.l with immediate reporting (e.g., unterminated strings).
Syntax Errors: Caught via Bison error productions, with custom messages (e.g., "Missing comma in object").
File Errors: Checked during CSV generation, with clear error messages.
Context: Errors include the offending token (yytext) for better debugging.


AST:

A hierarchical AST represents JSON structures, enabling flexible processing.
Nodes are dynamically allocated to handle arbitrary JSON input.
The cleanup function ensures no memory leaks.


CSV Output:

Relational format maps JSON objects to CSV rows, with keys as headers.
Supports nested structures by generating multiple CSV files (implementation in csv.c).
Output directory is configurable via --out-dir.



Limitations

Unicode Handling: The unescape_string function uses a placeholder (?) for \uXXXX escapes, lacking full Unicode support.
Complex JSON: Very large or deeply nested JSON inputs may stress memory allocation (mitigated by cleanup).
Error Recovery: The parser aborts on errors (YYABORT), which is sufficient for the assignment but could be extended for partial recovery.

Future Improvements

Implement full Unicode escape handling in unescape_string.
Add error recovery to continue parsing after errors.
Support JSON schema validation for stricter input checking.
Optimize AST memory usage for large inputs.

License
This project is for academic purposes and not distributed under a specific license.
