#let c-page = rgb("#f3f7ff")
#let c-ink = rgb("#1e2b4a")
#let c-card = rgb("#ffffff")
#let c-border = rgb("#d8e2ff")
#let c-banner = rgb("#2f5bdb")
#let c-soft = rgb("#f7fbff")

#set page(width: 18cm, height: auto, margin: (x: 1.1cm, y: 1cm), fill: c-page)
#set text(font: "Segoe UI", size: 10.8pt, fill: c-ink)
#show raw: set text(font: "Consolas", size: 9.3pt, fill: rgb("#17223d"))

#block(
  fill: c-banner,
  inset: 16pt,
  radius: 14pt,
  stroke: (paint: rgb("#284fbf"), thickness: 1pt),
)[
  #align(center)[
    #text(fill: white, size: 24pt, weight: "bold")[Expression Grammar]
    #v(0.3em)
    
  ]
]

#v(0.8em)

#block(
  fill: c-card,
  inset: 14pt,
  radius: 12pt,
  stroke: (paint: c-border, thickness: 1pt),
)[
  #block(
    fill: c-soft,
    inset: 9pt,
    radius: 9pt,
    stroke: (paint: rgb("#e5edff"), thickness: 0.8pt),
  )[
    #text(weight: "semibold", fill: rgb("#27469f"))[Covers]
    #parbreak()
    - integer and float literals
    - variable references
    - unary operators: `+`, `-`, `~`
    - binary numeric operators: `+`, `-`, `*`, `/`, `%`
    - bitwise operators: `&`, `^`, `|`
    - parenthesized expressions
    - function calls
  ]

  #v(0.7em)

]

#v(0.7em)

#block(
  fill: c-card,
  inset: 12pt,
  radius: 10pt,
  stroke: (paint: c-border, thickness: 1pt),
)[
  #text(weight: "semibold", fill: rgb("#27469f"))[Grammar (EBNF)]
  #v(0.45em)

  #block(
    fill: rgb("#f8fbff"),
    inset: 10pt,
    radius: 8pt,
    stroke: (paint: rgb("#e0e8ff"), thickness: 0.8pt),
  )[
```text
Expr        = BitOr ;

BitOr       = BitXor, { "|", BitXor } ;

BitXor      = BitAnd, { "^", BitAnd } ;

BitAnd      = Additive, { "&", Additive } ;

Additive    = Multiplicative, { ("+" | "-"), Multiplicative } ;

Multiplicative = Unary, { ("*" | "/" | "%"), Unary } ;

Unary       = { ("+" | "-" | "~") }, Postfix ;

Postfix     = Primary, [ "(", [ Arguments ], ")" ] ;

Arguments   = Expr, { ",", Expr } ;

Primary     = Literal
            | Identifier
            | "(", Expr, ")" ;

Literal     = Integer | Float ;

Integer     = Digit, { Digit } ;

Float       = Digit, { Digit }, ".", Digit, { Digit }
            | Digit, { Digit }, "." ;

Identifier  = Letter, { Letter | Digit | "_" } ;

Digit       = "0" | "1" | "2" | "3" | "4"
            | "5" | "6" | "7" | "8" | "9" ;

Letter      = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j"
            | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t"
            | "u" | "v" | "w" | "x" | "y" | "z"
            | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J"
            | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T"
            | "U" | "V" | "W" | "X" | "Y" | "Z" ;
```
  ]
]

#v(0.7em)

#block(
  fill: c-card,
  inset: 12pt,
  radius: 10pt,
  stroke: (paint: c-border, thickness: 1pt),
)[
  #text(weight: "semibold", fill: rgb("#27469f"))[Examples]
  #parbreak()
  - `1 + 2 * 3`
  - `x & y | z`
  - `-foo(4 + 5)`
  - `(a + b) % 2`
]

#v(0.7em)

#block(
  fill: c-card,
  inset: 12pt,
  radius: 10pt,
  stroke: (paint: c-border, thickness: 1pt),
)[
  #text(weight: "semibold", fill: rgb("#27469f"))[Parse Tree (Example)]
  #parbreak()
  #text(fill: rgb("#3d4f89"))[Expression]
  #parbreak()
  `~-foo(12, 3.5) + (a % 2) * b & c ^ d | e`
  #v(0.5em)

  #block(
    fill: rgb("#f8fbff"),
    inset: 10pt,
    radius: 8pt,
    stroke: (paint: rgb("#e0e8ff"), thickness: 0.8pt),
  )[
```text
Expr
└─ BitOr
   ├─ BitXor
   │  ├─ BitAnd
   │  │  ├─ Additive
   │  │  │  ├─ Multiplicative
   │  │  │  │  └─ Unary
   │  │  │  │     ├─ "~"
   │  │  │  │     ├─ "-"
   │  │  │  │     └─ Postfix
   │  │  │  │        ├─ Primary -> Identifier("foo")
   │  │  │  │        └─ "(" Arguments ")"
   │  │  │  │           ├─ Expr -> ... -> Literal -> Integer("12")
   │  │  │  │           └─ Expr -> ... -> Literal -> Float("3.5")
   │  │  │  ├─ "+"
   │  │  │  └─ Multiplicative
   │  │  │     ├─ Unary -> Postfix -> Primary -> "(" Expr ")"
   │  │  │     │  └─ Expr -> ... -> Additive
   │  │  │     │     ├─ Multiplicative -> Unary -> Postfix -> Primary -> Identifier("a")
   │  │  │     │     ├─ "%"
   │  │  │     │     └─ Multiplicative -> Unary -> Postfix -> Primary -> Literal -> Integer("2")
   │  │  │     ├─ "*"
   │  │  │     └─ Unary -> Postfix -> Primary -> Identifier("b")
   │  │  ├─ "&"
   │  │  └─ Additive -> Multiplicative -> Unary -> Postfix -> Primary -> Identifier("c")
   │  ├─ "^"
   │  └─ BitAnd -> Additive -> Multiplicative -> Unary -> Postfix -> Primary -> Identifier("d")
   ├─ "|"
   └─ BitXor -> BitAnd -> Additive -> Multiplicative -> Unary -> Postfix -> Primary -> Identifier("e")
```
  ]
]
