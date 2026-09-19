import { $ } from "bun";
import { rmSync, mkdirSync, existsSync, readdirSync, statSync, readFileSync, writeFileSync } from "fs";
import { readdir } from "fs/promises";
import { parseArgs } from "util";
import { resolve, join, basename, dirname } from "path";
import { unified } from "unified";
import remarkParse from "remark-parse";
import remarkGfm from "remark-gfm";
import remarkStringify from "remark-stringify";

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

  await $`bunx moxygen --groups --source-root ${CODE} --output "${PUBLIC}/docs/%s.md" ${XML_DIR}`;

  const apiIndex = resolve(PUBLIC, "docs", "api.md");
  if (!existsSync(apiIndex)) {
    throw new Error(
      "Moxygen did not generate docs/api.md; ungrouped API documentation would be missing.",
    );
  }

  /* ---------------- FIX DUPLICATE ---------------- */
  console.log("Removing Duplicate entries...");

  // helper to get raw text inside any node (formatted text, links, code, etc)
  function getNodeText(node: any): string {
    if (node.value) return node.value;
    if (node.children) return node.children.map(getNodeText).join("");
    return "";
  }

  const processor = unified()
    .use(remarkParse)
    .use(remarkGfm)
    .use(remarkStringify, { bullet: "-", fence: "`" });

  const docsDir = join(PUBLIC, "docs");
  const mdFiles = readdirSync(docsDir).filter((f) => f.endsWith(".md"));

  for (const file of mdFiles) {
    const filePath = join(docsDir, file);
    const text = readFileSync(filePath, "utf-8");
    const ast = processor.parse(text);

    // split AST into sections wherever there is a "---"
    const sections: any[][] = [[]];
    for (const child of ast.children) {
      if (child.type === "thematicBreak") {
        sections.push([]);
      } else {
        sections[sections.length - 1].push(child);
      }
    }

    // keep first section for each h4 heading and drop duplicate sections
    const seenH4 = new Set<string>();
    const keptSections: any[][] = [];

    for (const seg of sections) {
      const h4Node = seg.find((n) => n.type === "heading" && n.depth === 4);
      const h4Text = h4Node ? getNodeText(h4Node).trim() : null;

      if (!h4Text) {
        keptSections.push(seg);
      } else if (!seenH4.has(h4Text)) {
        seenH4.add(h4Text);
        keptSections.push(seg);
      }
    }

    // clean up duplicate table rows, moxygen puts ex. -1,-2 suffix on duplicate anchors
    for (const seg of keptSections) {
      let seenFuncs = new Set<string>();

      for (const node of seg) {
        if (node.type === "heading" && node.depth === 3) {
          seenFuncs = new Set();
        }

        if (node.type !== "table") continue;

        node.children = node.children.filter((row: any) => {
          let name: string | null = null;

          // find first anchor link in the table row
          for (const cell of row.children) {
            for (const c of cell.children || []) {
              if (c.type === "link") {
                name = c.url.replace(/^#/, "").replace(/-\d+$/, "");
                break;
              }
            }
            if (name) break;
          }

          if (!name) return true;
          if (seenFuncs.has(name)) return false;

          seenFuncs.add(name);
          return true;
        });
      }
    }

    // put everything back with "---" separators
    const newChildren: any[] = [];
    for (let i = 0; i < keptSections.length; i++) {
      if (i > 0) newChildren.push({ type: "thematicBreak" });
      newChildren.push(...keptSections[i]);
    }

    ast.children = newChildren;

    const output = processor.stringify(ast);
    writeFileSync(filePath, output);
  }

  console.log("Deduplication complete.");

  console.log("Docs build complete.");
}
