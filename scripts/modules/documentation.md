# Programming Guide: String, Array, Struct, Dictionary, and Functions

## String Methods
- `str.length()`: Returns the length of the string like `len(str)`.
- `str.upper()`: Converts the string to uppercase and modifies `str`.
- `str.lower()`: Converts the string to lowercase and modifies `str`.
- `str.find(Search,[Start])`: Finds the index of the first occurrence of 'search' after 'start' within `str`. 'Start' is optional.
- `str.join(array)`: Concatenates array elements, separated by `str`.
- `SplitWords(str, delimiter := " ", max_split := -1)`: Splits `str` by delimiter.
- `str.format()`: Formats string with various options:
  - Hex, octal, binary representation (`{:x}`, `{:o}`, `{:b}`).
  - Notation prefix with `#` (e.g., `"{:#x}".format(10)` -> `"0xa"`).
  - Simple parameters and implicit unpacking by order.
  - Explicit parameter specification (e.g., `"{2} hits {1}".format("Bob", "John")`).
  - Accessing object members (e.g., `"{1.spell_name}".format(spell)`).
- `$"..."`: Added: Interpolated string expression:.
  - This syntax allows joining multiple components into a string. Inside `"..."`, you can include:
    - Normal strings.
    - Escaped characters, e.g., `\n`.
    - `{expression [: format]}`, e.g., `{who.name}` or `{who.graphic : #x}`.
    - `{{` and `}}` to represent `{` and `}` characters.
  - Example:
    - `$"Hello, {who.name}! You are player #{who in EnumerateOnlineCharacters()} online. Your graphic is {who.graphic : #x}."`
    - Replaces complex concatenations or `.format` method.

## Array Functions
- `Min(x, y:=0)`: Returns the minimum of x and y or the lowest element in an array.
- `Max(x, y:=0)`: Returns the maximum of x and y or the highest element in an array.
- `array.Size()`: Returns the number of elements in the array.
- `array.Insert(index, value)`: Inserts a value at the specified index.
- `array.Erase(index)`: Deletes the element at the specified index.
- `array.Shrink(nelems)`: Erases all but the first `nelems` elements.
- `array.Append(value)`: Adds an element to the end of the array.
- `array.Reverse()`: Reverses the order of the array.
- `array.Sort()`: Sorts the array.
- `array.cycle([count])`: Rotates the array like a conveyor belt.
- `array[randomIndex]`: Accesses a random entry in the array.
- `_elem_iter`: Index in `foreach` and `while` loops.

## Struct Enhancements
- `struct .?` and `.-`: Shortcuts for `.exists()` and `.erase()` respectively.

## Dictionary Operations
- `dictionary.size()`: Returns the number of elements.
- `dictionary.erase(key)`: Erases an element.
- `dictionary.insert(key, value)`: Inserts an element.
- `dictionary.exists(key)`: Checks if a key exists.
- `dictionary.keys()`: Returns an array of all keys.
- Example: Nested dictionary with options.

## Functions Update (02-05-2018)
- Operator `@`: Used to create a function object.
  - `func.call(..)`: Calls the function with specified parameters.
  - Limitations: The function object can only be executed in the same script instance and cannot be saved/loaded.
  - Use Case: Creating generic item loops and fast function tables.

  ```php
  function genericItemLoop(func)
    for item in ...
      // complicated checks
      func.call(item);
    endfor
  endfunction
  function changecolor(item)
    item.color:=6;
  endfunction
  function changeY(item)
    item.y:=item.y+1;
  endfunction
  ...
  genericItemLoop(@changecolor);
  genericItemLoop(@changeY);

  // (fast) function table, instead of "if" cascade
  var look=dictionary;
  look["blubb"]:=@doSomething
  look["blah"]:=@doSomethingElse
  ...
  look[mykey].call();
  ```
  
- **Added: Conditional Operator** - `condition ? exprIfTrue : exprIfFalse`
  - **Description:** 
    - This operator allows for efficient evaluation of two expressions based on a condition.
    - The syntax `condition ? exprIfTrue : exprIfFalse` evaluates the condition:
      - If the condition is true, `exprIfTrue` is evaluated and returned.
      - If the condition is false, `exprIfFalse` is evaluated and returned.

# GET ENVIRONMENT VARIABLE
- **Added Function:** `os::GetEnvironmentVariable(name:="")`
  - **Description:** Returns the string value of a given environment variable `name`.
  - **Usage:**
    - If `name` is specified, returns the value of that specific environment variable.
    - If `name` is empty (`""`), returns a Dictionary containing all allowed environment variables.