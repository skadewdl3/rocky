# Rocky Slides

This project compiles Typst slide decks from `src/<folder>/main.typ` into PDF files under `slides/`.

## Generate Slides

Use the Pixi task defined in `pixi.toml`:

```bash
pixi run generate
```

This runs:

```bash
python generate_slides.py
```

When no folder names are provided, all slide folders in `src/` (that contain `main.typ`) are compiled.

## Generate Specific Decks

To compile only selected folders, pass folder names directly to the script:

```bash
pixi run python generate_slides.py 0-initial-issues
pixi run python generate_slides.py 0-initial-issues 1-community-kickoff
```

Outputs are written to:

- `slides/0-initial-issues.pdf`
- `slides/1-community-kickoff.pdf`
