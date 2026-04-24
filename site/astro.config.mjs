// @ts-check
import { defineConfig } from 'astro/config';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import tailwindcss from '@tailwindcss/vite';
import rehypePrettyCode from "rehype-pretty-code";

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
      }]
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
