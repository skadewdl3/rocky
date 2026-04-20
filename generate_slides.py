from __future__ import annotations

import argparse
import sys
from pathlib import Path

import typst


def discover_all_folders(source_dir: Path) -> list[str]:
    return sorted(
        entry.name
        for entry in source_dir.iterdir()
        if entry.is_dir() and (entry / "main.typ").is_file()
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compile Typst slide decks from src/<folder>/main.typ into slides/<folder>.pdf."
        )
    )
    parser.add_argument(
        "folders",
        nargs="*",
        help="Folder names under src/ to compile. If omitted, compile all folders.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    repo_root = Path(__file__).resolve().parent
    source_dir = repo_root / "src"
    slides_dir = repo_root / "slides"
    fonts_dir = repo_root / "fonts"

    if not source_dir.is_dir():
        print(f"Error: source directory does not exist: {source_dir}", file=sys.stderr)
        return 1

    slides_dir.mkdir(parents=True, exist_ok=True)

    all_folders = discover_all_folders(source_dir)
    if args.folders:
        folders_to_build = args.folders
    else:
        folders_to_build = all_folders

    if not folders_to_build:
        print("No folders to compile.")
        return 0

    unknown_folders = [
        folder for folder in folders_to_build if folder not in all_folders
    ]
    if unknown_folders:
        print(
            "Error: folder(s) not found or missing main.typ under src/: "
            + ", ".join(unknown_folders),
            file=sys.stderr,
        )
        return 1

    for folder_name in folders_to_build:
        input_file = source_dir / folder_name / "main.typ"
        output_file = slides_dir / f"{folder_name}.pdf"

        print(f"Compiling {input_file} -> {output_file}")
        typst.compile(
            input=str(input_file),
            output=str(output_file),
            root=str(repo_root),
            font_paths=[str(fonts_dir)],
        )

    print(f"Done. Generated {len(folders_to_build)} PDF(s) in {slides_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
