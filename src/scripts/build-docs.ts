import { $ } from "bun";
import { rmSync, mkdirSync, existsSync, readdirSync, statSync } from "fs";
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
const DOCS = ensure("docs");
const SLIDES = resolve(WORKDIR, "slides");
const CODE = resolve(WORKDIR, "code");
const SLIDES_DEST = resolve(ROOT, "public");
const DOCS_DEST = resolve(SRC, "content", "docs");

const REPO_URL = "https://github.com/skadewdl3/rocky";
const SLIDES_BRANCH = "slides";
const CODE_BRANCH = "main";

const TYPST_URL =
  "https://github.com/typst/typst/releases/download/v0.14.2/typst-x86_64-unknown-linux-musl.tar.xz";
let TYPST_BIN = null;
const DOXYGEN_URL = "";
let DOXYGEN_BIN = "";

// clean
if (!args["skip-clean"]) {
  clean(WORKDIR);
}

/* ---------------- INSTALL TYPST ---------------- */
if (!args["skip-typst"] && !args["skip-downloads"] && !args["skip-slides"]) {
  console.log("Installing Typst...");
  await $`
    curl -L ${TYPST_URL} -o ${resolve(WORKDIR, "typst.tar.xz")}
  `;
  await $`tar -xJf ${resolve(WORKDIR, "typst.tar.xz")} -C ${WORKDIR}`;
}

let TYPST_BIN_DIR = findDir({ baseDir: WORKDIR, prefix: "typst" });
if (!TYPST_BIN_DIR) throw new Error("Typst binary not found");
TYPST_BIN = resolve(TYPST_BIN_DIR, "typst");
await $`chmod +x ${TYPST_BIN}`;

if (!args["skip-doxygen"] && !args["skip-downloads"] && !args["skip-docs"]) {
  /* ---------------- INSTALL DOXYGEN ---------------- */
  console.log("Installing Doxygen...");
  await $`
    curl -L https://www.doxygen.nl/files/doxygen-1.10.0.linux.bin.tar.gz -o doxygen.tar.gz
  `;
  await $`tar -xzf doxygen.tar.gz`;
  await $`mv doxygen-*/bin/doxygen doxygen`;
  await $`chmod +x doxygen`;
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

    const out = `${SLIDES_DEST}/${name}.pdf`;
    console.log(`→ ${name}.pdf`);

    await $`${TYPST_BIN} compile ${entry} ${out} --root ${src}`;

    slidesMap[`/${name}`] = config;
  }

  await $`echo '${JSON.stringify(slidesMap, null, 2)}' > ${SLIDES_DEST}/slides.json`;

  console.log("Slides build complete.");
}

if (!args["skip-doxygen"] && !args["skip-build"] && !args["skip-docs"]) {
  /* ---------------- DOXYGEN ---------------- */
  console.log("Running Doxygen...");

  const XML_OUT = "temp/doxygen-out";

  // later, add a Doxyfile to main branch, and control this through it
  await $`
  ./doxygen Doxyfile \
    INPUT=${CODE}/src ${CODE}/include \
    OUTPUT_DIRECTORY=${XML_OUT}
`;

  /* ---------------- MOXYGEN ---------------- */
  console.log("Running Moxygen...");

  const XML_DIR = `${CODE}/docs/xml`;

  await $`bunx moxygen --output ${DOCS_DEST} ${XML_DIR}`;

  console.log("Docs build complete.");
}
