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