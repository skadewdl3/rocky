// @ts-check
import { defineConfig } from 'astro/config';
import { fileURLToPath } from 'node:url';

import cloudflare from '@astrojs/cloudflare';

import tailwindcss from '@tailwindcss/vite';

const micromarkProductionEntry = fileURLToPath(
  new URL('./node_modules/micromark/index.js', import.meta.url)
);

// https://astro.build/config
export default defineConfig({
  adapter: cloudflare(),

  vite: {
    resolve: {
      alias: [
        {
          find: /^micromark$/,
          replacement: micromarkProductionEntry
        }
      ]
    },
    plugins: [tailwindcss()]
  }
});
