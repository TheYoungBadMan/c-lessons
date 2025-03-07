# Грамматика

Этот файл содержит описание грамматики интерпретатора математических выражений в формате EBNF с регулярными выражениями.

## Лексер

Входные данные - строка, содержащая математическое выражение.
Выходные данные - список токенов.

### Токены

- `id` - идентификатор (переменная или функция)
- `number` - число (целое или дробное)
- `operator` - оператор или пунктуатор

Пробельные символы игнорируются.

### Лексические правила

```ebnf
<id> ::= <letter> (<letter> | <digit>)*
<number> ::= <integer> | <float>
<operator> ::= '+' | '-' | '*' | '/' | '^' | ',' | '(' | ')'

<integer> ::= <digit>+
<float> ::= <integer> '.' | <integer> '.' <integer>

<letter> ::= [a-zA-Z_]
<digit> ::= [0-9]
```

## Парсер

Входные данные - список токенов.
Выходные данные - дерево разбора (AST).

### AST

- `binary` - бинарная операция
- `unary` - унарная операция
- `function` - функция с переменным числом аргументов
- `group` - выражение в скобках
- `var` - переменная
- `number` - число (дробное по умолчанию)

### Синтаксические правила

```ebnf
<expression> ::= <term> | <term> <sum-op> <expression>
<term> ::= <factor> | <factor> <product-op> <term>
<factor> ::= <unary> | <unary> <power-op> <factor>
<unary> ::= <unary-op> <unary> | <primary>
<primary> ::= <function> | <group> | <var> | <number>
<function> ::= <id> '(' <arg-list>? ')'
<arg-list> ::= <expression> | <expression> ',' <arg-list>
<group> ::= '(' <expression> ')'
<var> ::= <id>

<sum-op> ::= '+' | '-'
<product-op> ::= '*' | '/'
<power-op> ::= '^'
<unary-op> ::= '+' | '-'
```
