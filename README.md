# JSON to Relational CSV Converter

This project implements a JSON-to-Relational-CSV converter using **Flex** and **Bison**. It parses JSON input, constructs an Abstract Syntax Tree (AST), and generates relational CSV files. The tool supports robust error handling, and includes options to print the AST or specify an output directory for CSV outputs.

---

## ✍️ Authors

- **Tauha Imran** (22i-1239)  
- **Hussain Ali** (22i-0902)

---

## ⚙️ Build Instructions

### ✅ Prerequisites

- **Operating System:** Linux (tested on Ubuntu)
- **Required Tools:**
  - GCC (`gcc`)
  - Bison (`bison`)
  - Flex (`flex`)
  - Make (`make`)
- **Required Libraries:**
  - Flex development library (`libfl-dev`)

Install dependencies (on Ubuntu):
```bash
sudo apt-get update
sudo apt-get install gcc bison flex libfl-dev make
```

---

## 📁 Project Structure

```
src/
├── parser.y       # Bison grammar for JSON parsing
├── scanner.l      # Flex lexer for JSON tokens
├── main.c         # Entry point and CLI handling
├── ast.h/.c       # AST node definitions and manipulation
├── csv.h/.c       # CSV generation and file handling
└── Makefile       # Build script
```

Test files:
- `test1.json`
- `test_error2.json`
- `test_error3.json`

---

## 🏗️ Build Steps

```bash
cd ~/cc-assignment/4/vb/JSON-to-Relational-CSV-with-Flex---Yacc/src
make clean   # optional
make         # builds the 'json2relcsv' executable
```

---

## 🚀 Run Instructions

### 📌 Usage

```bash
./json2relcsv [--print-ast] [--out-dir <directory>] < <input.json>
```

### 🔧 Options

- `--print-ast` : Print the AST instead of generating CSV files.
- `--out-dir <directory>` : Specify output directory (default: current directory).

### 🧪 Examples

#### ✅ Valid JSON Input
```bash
echo '{ "id": 1, "name": "Ali", "age": 19 }' > test1.json
./json2relcsv < test1.json
```
Generates `people.csv`:
```
id,name,age
1,Ali,19
```

#### ❌ Lexical Error (Unterminated String)
```bash
echo '{ "id": 1 "name": "Ali }' > test_error2.json
./json2relcsv < test_error2.json
```
Output:
```
Error: Unterminated string at line 3, column 11: "Ali
```

#### ❌ Syntax Error (Missing Comma)
```bash
echo '{ "id": 1 "name": "Ali" }' > test_error3.json
./json2relcsv < test_error3.json
```
Output:
```
Error: Missing comma in object at line 3, column 5: "name"
```

#### ❌ File Error (Invalid Output Directory)
```bash
./json2relcsv --out-dir /invalid/dir < test1.json
```
Output:
```
Error: Cannot open file: /invalid/dir/root.csv
```

#### 🌲 Print AST
```bash
./json2relcsv --print-ast < test1.json
```

---

## 🧪 Testing

Run test cases:
```bash
./json2relcsv < test1.json
./json2relcsv < test_error2.json
./json2relcsv < test_error3.json
./json2relcsv --out-dir /invalid/dir < test1.json
./json2relcsv --print-ast < test1.json
```

---

## 🧠 Design Notes

### 🔄 Overview

1. **Lexing**: Tokenizes input JSON using Flex.
2. **Parsing**: Builds AST with Bison.
3. **Processing**: Converts AST into relational CSV.
4. **Error Handling**: Accurate line/column error reporting.

### 🔍 Key Components

#### **Flex Lexer (scanner.l)**
- Reentrant (`%option reentrant`, `bison-bridge`, `bison-locations`)
- Recognizes JSON tokens (objects, arrays, strings, numbers, etc.)
- Handles string escapes and invalid input with error reporting

#### **Bison Parser (parser.y)**
- Reentrant parser using `%define api.pure full`
- Builds AST via semantic actions (`create_object_node`, etc.)
- Error productions report syntax issues

#### **AST (ast.h / ast.c)**
- Nodes for JSON values: object, array, string, number, etc.
- Functions for creation, linking, and printing
- Managed memory with cleanup

#### **CSV Generator (csv.h / csv.c)**
- Traverses AST to produce CSV files
- Handles nested structures as separate CSVs
- Customizable output directory

#### **Main (main.c)**
- Argument parsing and scanner setup
- Dispatch to print AST or generate CSV
- Global cleanup

---

## 💡 Design Decisions

- **Reentrant Design**: Ensures thread safety and modular parsing.
- **Detailed Error Reporting**: Custom messages with context token (`yytext`), line, and column.
- **AST Flexibility**: Easily supports recursive/nested structures.
- **CSV Output Structure**: Simple flat representation, expandable for nested records.

---

## ⚠️ Known Limitations

- `\uXXXX` Unicode escape handling is limited (uses `?` as placeholder).
- Deeply nested JSON may affect memory usage.
- Parsing halts on first error (no recovery).

---

## 🌱 Future Improvements

- Full Unicode support (`\uXXXX` handling).
- Error recovery to continue after parsing issues.
- JSON Schema validation for input.
- Optimize memory footprint.

---

## 📄 License

This project is for **academic purposes only** and not licensed for public distribution.
