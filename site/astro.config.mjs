// @ts-check
import { defineConfig } from 'astro/config';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import tailwindcss from '@tailwindcss/vite';
import rehypePrettyCode from "rehype-pretty-code";
import { createSlugger } from "./src/lib/heading-utils.js";

function rehypeHeadingIds() {
  return (tree) => {
    const slugify = createSlugger();

    const visit = (node) => {
      if (!node || typeof node !== "object") {
        return;
      }

      if (node.type === "element" && (node.tagName === "h2" || node.tagName === "h3")) {
        const text = getText(node).trim();

        if (text) {
          node.properties ??= {};
          node.properties.id = node.properties.id ?? slugify(text);
        }
      }

      if (Array.isArray(node.children)) {
        for (const child of node.children) {
          visit(child);
        }
      }
    };

    visit(tree);
  };
}

function getText(node) {
  if (!node || typeof node !== "object") {
    return "";
  }

  if (typeof node.value === "string") {
    return node.value;
  }

  if (!Array.isArray(node.children)) {
    return "";
  }

  return node.children.map(getText).join(" ");
}

// https://astro.build/config
const siteDir = dirname(fileURLToPath(import.meta.url));
const repoRoot = resolve(siteDir, '..');

export default defineConfig({
  markdown: {
    syntaxHighlight: false,
    rehypePlugins: [
      [rehypePrettyCode, {
        theme: "github-dark",
        keepBackground: false,
        defaultLang: "txt"
      }],
      rehypeHeadingIds
    ]
  },
  vite: {
    plugins: [tailwindcss()],
    server: {
      fs: {
        allow: [repoRoot]
      }
    }
  }
});
