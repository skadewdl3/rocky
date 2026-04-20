#let codeblock_bg(body, fill: rgb("#eee")) = [
  #show raw.where(block: true): set block(
    fill: fill,
  )
  #body
]
