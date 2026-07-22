import { $ } from "bun";
import { rmSync, mkdirSync, existsSync, readdirSync, statSync, readFileSync, writeFileSync } from "fs";
import { readdir } from "fs/promises";
import { parseArgs } from "util";
import { resolve, join, basename, dirname } from "path";

function ensure(path: string) {
  let dir = resolve(path);
  if (!existsSync(dir)) mkdirSync(dir, { recursive: true });
  return dir;
}

function clean(path: string) {
  if (existsSync(path)) rmSync(path, { recursive: true });
  return ensure(path);
}

type FindDirOptions = {
  baseDir?: string;
  prefix?: string;
  suffix?: string;
};

export function findDir({
  baseDir = ".",
  prefix = "",
  suffix = "",
}: FindDirOptions): string | null {
  const entries = readdirSync(baseDir);

  for (const name of entries) {
    if (name.startsWith(prefix) && name.endsWith(suffix)) {
      const full = join(baseDir, name);
      if (statSync(full).isDirectory()) {
        return full;
      }
    }
  }

  return null;
}
const { values: args, positionals } = parseArgs({
  args: Bun.argv,
  options: {
    "skip-typst": {
      type: "boolean",
      default: false,
    },
    "skip-doxygen": {
      type: "boolean",
      default: false,
    },
    "skip-downloads": {
      type: "boolean",
      default: false,
    },
    "skip-clones": {
      type: "boolean",
      default: false,
    },
    "skip-build": {
      type: "boolean",
      default: false,
    },
    "skip-slides": {
      type: "boolean",
      default: false,
    },
    "skip-docs": {
      type: "boolean",
      default: false,
    },
    "skip-clean": {
      type: "boolean",
      default: false,
    },
  },
  strict: true,
  allowPositionals: true,
});

const WORKDIR = ensure("temp");
const SRC = resolve("src");
const ROOT = resolve(SRC, "..");
const SLIDES = resolve(WORKDIR, "slides");
const CODE = resolve(WORKDIR, "code");
const PUBLIC = resolve(ROOT, "public");
ensure(resolve(PUBLIC, "docs"));

const REPO_URL = "https://github.com/skadewdl3/rocky";
const SLIDES_BRANCH = "slides";
const CODE_BRANCH = "main";

const TYPST_URL =
  "https://github.com/typst/typst/releases/download/v0.14.2/typst-x86_64-unknown-linux-musl.tar.xz";
const DOXYGEN_URL =
  "https://github.com/doxygen/doxygen/releases/download/Release_1_17_0/doxygen-1.17.0.linux.bin.tar.gz";
let TYPST_BIN = null;
let DOXYGEN_BIN = null;
let TYPST_BIN_DIR = null;
let DOXYGEN_BIN_DIR = null;

// clean
if (!args["skip-clean"]) {
  clean(WORKDIR);
  clean(resolve(SRC, "content"));
}

/* ---------------- INSTALL TYPST ---------------- */
if (!args["skip-typst"] && !args["skip-slides"]) {
  if (!args["skip-downloads"]) {
    console.log("Downloading Typst...");
    await $`
      curl -L ${TYPST_URL} -o ${resolve(WORKDIR, "typst.tar.xz")}
    `;
    await $`tar -xJf ${resolve(WORKDIR, "typst.tar.xz")} -C ${WORKDIR}`;
  }
  TYPST_BIN_DIR = findDir({ baseDir: WORKDIR, prefix: "typst" });
  if (!TYPST_BIN_DIR) throw new Error("Typst binary not found");
  TYPST_BIN = resolve(TYPST_BIN_DIR, "typst");
  await $`chmod +x ${TYPST_BIN}`;
}

/* ---------------- INSTALL DOXYGEN ---------------- */
if (!args["skip-doxygen"] && !args["skip-docs"]) {
  if (!args["skip-downloads"]) {
    console.log("Downloading Doxygen...");
    await $`curl -L ${DOXYGEN_URL} -o ${resolve(WORKDIR, "doxygen.tar.gz")}`;
    await $`tar -xvf ${resolve(WORKDIR, "doxygen.tar.gz")} -C ${WORKDIR}`;
  }
  DOXYGEN_BIN_DIR = findDir({ baseDir: WORKDIR, prefix: "doxygen" });
  DOXYGEN_BIN = resolve(DOXYGEN_BIN_DIR, "bin", "doxygen");
  await $`chmod +x ${DOXYGEN_BIN}`;
}

/* ---------------- CLONE ---------------- */
if (!args["skip-slides"] && !args["skip-clones"]) {
  console.log("Cloning slides...");
  await $`git clone --depth=1 --branch ${SLIDES_BRANCH} ${REPO_URL} ${SLIDES}`;
}

if (!args["skip-docs"] && !args["skip-clones"]) {
  console.log("Cloning code...");
  await $`git clone --depth=1 --branch ${CODE_BRANCH} ${REPO_URL} ${CODE}`;
}

/* ---------------- BUILD TYPST ---------------- */
if (!args["skip-typst"] && !args["skip-build"] && !args["skip-slides"]) {
  console.log("Building slides...");

  const src = resolve(SLIDES, "src");
  let slidesMap: Record<string, any> = {};
  const entries = readdirSync(src);

  for (const name of entries) {
    const dir = join(src, name);

    if (!statSync(dir).isDirectory()) continue;

    const entry = join(dir, "main.typ");

    if (!existsSync(entry)) continue;

    const config =
      await $`${TYPST_BIN} query ${entry} "<docs-config>" --root ${src} --field value --one`.json();

    const out = `${PUBLIC}/${name}.pdf`;
    console.log(`→ ${name}.pdf`);

    await $`${TYPST_BIN} compile ${entry} ${out} --root ${src}`;

    slidesMap[`/${name}`] = config;
  }

  // Sort slidesMap alphabetically by key
  const sortedSlidesMap = Object.fromEntries(
    Object.entries(slidesMap).sort((a, b) => a[0].localeCompare(b[0])),
  );
  await $`echo '${JSON.stringify(sortedSlidesMap, null, 2)}' > ${PUBLIC}/slides.json`;

  console.log("Slides build complete.");
}

if (!args["skip-doxygen"] && !args["skip-build"] && !args["skip-docs"]) {
  /* ---------------- DOXYGEN ---------------- */
  console.log("Running Doxygen...");

  // later, add a Doxyfile to main branch, and control this through it
  let DOXYFILE =
    await $`cat Doxyfile ; echo "GENERATE_XML=YES"; echo "OUTPUT_DIR=${resolve(CODE, "docs")}";`
      .cwd(CODE)
      .text();
  console.log(DOXYFILE);
  await $`echo '${DOXYFILE}' | ${DOXYGEN_BIN} -`.cwd(CODE);

  let XML_DIR = resolve(CODE, "docs", "xml");
  /* ---------------- MOXYGEN ---------------- */
  // console.log("Running Moxygen...");

  // const XML_DIR = `${CODE}/docs/xml`;

  await $`bunx moxygen --groups --output "${PUBLIC}/docs/%s.md" ${XML_DIR}`;

  /* ---------------- FIX DUPLICATE ---------------- */
  console.log("Removing Duplicate entries...");

  const docsDir = join(PUBLIC, "docs");
  const mdFiles = readdirSync(docsDir).filter((f) => f.endsWith(".md"));

  for (const file of mdFiles) {
    const filePath = join(docsDir, file);
    const seenHeaders = new Set();
    const seenFuncs = new Set();
    const lines = readFileSync(filePath, "utf-8").split("\n");

    // group lines into segments separated by "---"
    const segments = [[]];
    for (const line of lines) {
      if (line.trim() === "---") segments.push([line]);
      else segments[segments.length - 1].push(line);
    }

    // filter segments with duplicate h4 headings
    const kept = segments.flatMap((seg) => {
      const header = seg.find((l) => l.trim().startsWith("#### "));
      if (!header) return [seg];

      const name = header.trim().slice(5).trim();
      if (!seenHeaders.has(name)) {
        seenHeaders.add(name);
        return [seg];
      }

      // keep only ## content if present
      const h2 = seg.findIndex((l) => l.trim().startsWith("## "));
      return h2 !== -1 ? [seg.slice(h2)] : [];
    });

    // remove duplicate description lines within each segment
    for (let i = 0; i < kept.length; i++) {
      const seen = new Set();
      let inCodeBlock = false;

      kept[i] = kept[i].filter((line) => {
        const trimmed = line.trim();

        // make sure code blocks dont get messed with
        if (trimmed.startsWith("```")) inCodeBlock = !inCodeBlock;
        if (!trimmed || inCodeBlock) return true;
        if (seen.has(trimmed)) return false;

        seen.add(trimmed);
        return true;
      });
    }

    // fix table markdown duplication
    const keptLines = [];
    for (const line of kept.flat()) {
      // reset seen stuff on new section header
      if (line.trim().startsWith("### ")) seenFuncs.clear();

      if (!line.startsWith("|")) {
        keptLines.push(line);
        continue;
      }

      const s = line.indexOf("](#");
      if (s === -1) {
        keptLines.push(line);
        continue;
      }

      const e = line.indexOf(")", s);

      // remove trailing -N suffix (ex. jit_init-1 and jit_init duplicates)
      const fnName = line.slice(s + 3, e).replace(/-\d+$/, ""); // regex to remove -N suffix
      if (seenFuncs.has(fnName)) continue;

      seenFuncs.add(fnName);
      keptLines.push(line);
    }

    writeFileSync(filePath, keptLines.join("\n"));
  }

  console.log("Deduplication complete.");
}
