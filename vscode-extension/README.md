# rbasic VS Code Extension

This folder contains the VS Code extension files for rbasic syntax highlighting and basic IntelliSense support.

## Installation

1. **Copy the entire `vscode-extension` folder to your VS Code extensions directory:**
   ```
   %APPDATA%\Code\User\extensions\rbasic-language-1.0.0\
   ```

2. **Restart VS Code**

3. **Open any `.bas` or `.rbasic` file** to see syntax highlighting in action!

## What's Included

### Files
- `package.json` - Extension manifest defining language support
- `language-configuration.json` - Language settings for brackets, comments, indentation
- `syntaxes/rbasic.tmLanguage.json` - TextMate grammar for syntax highlighting

### Features
- ✅ **Syntax Highlighting**: Keywords, functions, strings, comments, numbers
- ✅ **Auto-Closing**: Brackets, braces, parentheses, quotes
- ✅ **Comment Toggle**: Ctrl+/ for line comments, Shift+Alt+A for block comments
- ✅ **Smart Indentation**: Automatic indenting for code blocks
- ✅ **Bracket Matching**: Highlight matching brackets/braces
- ✅ **Word-based IntelliSense**: Basic auto-completion from document words

## Language Support

### Highlighted Elements
- **Control Keywords**: `if`, `else`, `for`, `while`, `function`, `return`
- **Declaration Keywords**: `var`, `dim`, `struct`, `type`, `as`
- **Built-in Functions**: 
  - Math: `sqrt`, `sin`, `cos`, `randomise`, `pi`
  - String: `len`, `left`, `right`, `mid`, `str`, `val`
  - Graphics: `graphics_mode`, `set_colour`, `draw_rect`, `draw_circle`, `draw_text`
  - I/O: `print`, `input`, `clear_screen`, `sleep_ms`
  - System: `key_pressed`, `quit_requested`, `get_ticks`
- **Data Types**: `integer`, `double`, `string`, `boolean`, `thread`, `mutex`
- **Operators**: `+`, `-`, `*`, `/`, `==`, `!=`, `<=`, `>=`, `and`, `or`, `not`
- **Literals**: Numbers, strings, booleans (`true`, `false`)

### File Extensions
- `.bas` - Traditional BASIC files
- `.rbasic` - rbasic-specific files

## Example Syntax

```rbasic
// Comments are highlighted
function fibonacci(n as integer) as integer {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

var result = fibonacci(10);
print("Fibonacci(10) =", result);

// Graphics example with British spelling
graphics_mode(800, 600);
set_colour(255, 0, 0);  // Red colour
draw_rect(100, 100, 200, 150, true);
draw_text(110, 110, "Hello, rbasic!");
refresh_screen();
```

## Manual Installation Steps

1. **Navigate to VS Code extensions folder:**
   ```
   Windows: %APPDATA%\Code\User\extensions\
   macOS: ~/.vscode/extensions/
   Linux: ~/.vscode/extensions/
   ```

2. **Create folder:** `rbasic-language-1.0.0`

3. **Copy all files from this `vscode-extension` folder** into the new directory

4. **Restart VS Code**

## Development Testing

To test changes to the grammar:

1. **Open this folder in VS Code**
2. **Press F5** to launch Extension Development Host
3. **Open a .bas file** in the development window to test highlighting
4. **Make changes to the grammar and reload the window** to see updates

## Customization

### Adding New Keywords
Edit `syntaxes/rbasic.tmLanguage.json` and add to the appropriate pattern:

```json
{
    "name": "keyword.control.new.rbasic",
    "match": "\\b(newkeyword|anotherkeyword)\\b"
}
```

### Adding New Functions
Add to the builtin-functions patterns:

```json
{
    "name": "support.function.builtin.category.rbasic",
    "match": "\\b(new_function|another_function)\\b"
}
```

### Changing Colors
The actual colors depend on your VS Code theme. Different themes will style the scopes differently:
- `keyword.control` - Usually purple/blue
- `support.function.builtin` - Usually light blue
- `string.quoted` - Usually orange/yellow
- `comment.line` - Usually green

Enjoy syntax-highlighted rbasic programming! 🎨