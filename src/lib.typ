#import "@preview/codly:1.3.0": *

#let default-codly-config = (
  number-format: none,
  zebra-fill: none,
)

#let with-codly-config(body, ..args) = [
  #codly(..args)
  #body
  #place(hide[
    #codly-reset()
    #codly(..default-codly-config)
    ```

    ```
  ])
]
