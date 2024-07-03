### Numeric literals

Every numeric literal can contain underscores `_` or single quotes `'` to improve readability – those characters
are ignored by the compiler. They must be placed between two digits. All integer numeric literals are `i64` by default, floats are `f64`.
```ebnf
integer_literal ::= decimal_literal | hexadecimal_literal | binary_literal | octal_literal

decimal_digit ::= "0" ... "9"
decimal_literal ::= 
    | "0" 
    | ( "1" ... "9" ) { decimal_digit }

binary_digit ::= "0" | "1"
binary_literal ::= "0" ( "b" | "B" ) { binary_digit }

octal_digit ::= "0" ... "7"
octal_literal ::= "0" ( "o" | "O" ) { octal_digit }

hexadecimal_digit ::= "0" ... "9" | "a" ... "f" | "A" ... "F"
hexadecimal_literal ::= "0" ( "x" | "X" ) { hexadecimal_digit }
```

### Numeric types
- Integers
  - `i8` (8-bit signed integer)
  - `i32` (32-bit signed integer)
  - `i64` (64-bit signed integer)
- Floating point
  - `f32` (32-bit floating point)
  - `f64` (64-bit floating point)

### Possible escape sequences within a string literal

| Sequence     | Output             |
|--------------|--------------------|
| ``\\``       | Backslash          |
| ``\'``       | Single quote       |
| ``\"``       | Double quote       |
| ``\n``       | Newline            |
| ``\r``       | Carriage return    |
| ``\t``       | Horizontal tab     |
| ``\0``       | Null character     |
| ``\xhh``     | Hexadecimal byte   |
| ``\u{hhhh}`` | Unicode code point |

Where ``h`` is a hexadecimal digit. Unicode code points are expanded into UTF-8 bytes.