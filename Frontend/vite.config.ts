import fs from "node:fs";
import path from "node:path";
import mime from "mime-types";
import { defineConfig } from "vite";
import type { Plugin } from "vite";
import react from "@vitejs/plugin-react";
import tsconfigPaths from "vite-tsconfig-paths";

export async function listDirectory(baseDirPath: string, fullPaths: boolean = true) {
  const result: string[] = [];

  const list = async (dirPath: string) => {
    const fullDirPath = path.join(baseDirPath, dirPath);

    for (const subRelativePath of await fs.promises.readdir(fullDirPath)) {
      const fullPath = path.join(fullDirPath, subRelativePath);
      const subPath = path.join(dirPath, subRelativePath);
  
      const stats = await fs.promises.stat(fullPath);
  
      if (stats.isDirectory())
        await list(subPath);
      else
        result.push(fullPaths ? fullPath : subPath);
    }
  };

  await list(".");

  return result;
}

function bundleWebAssets(): Plugin {
  return {
    name: "bundleWebAssets",
    apply: "build",

    async closeBundle() {
      const distDir = path.resolve(__dirname, "dist");

      if (!fs.existsSync(distDir))
        throw new Error("dist directory not found");

      const files = await listDirectory(distDir);
      files.sort();

      const outputFilePath = path.resolve(__dirname, "bundle/WebAssetsBundle.inc");

      let generatedAssets = "";
      
      let assetId = 0;
      for (const assetPath of files) {
        const pathname = '/' + path.relative(distDir, assetPath).replaceAll('\\', '/');
        const mimeType = mime.lookup(pathname) || "application/octet-stream";

        generatedAssets +=
`{
  static char _asset${assetId}_data[] = {
#embed <${path.resolve(assetPath).replaceAll('\\', '/')}>
  };
  auto pathname = std::string { "${pathname}" };
  m_assets.emplace(pathname, std::make_unique<WebAsset>(
    ${assetId}, pathname, "${mimeType}", _asset${assetId}_data, sizeof(_asset${assetId}_data)
  ));
}\n`;

        ++assetId;
      }

      await fs.promises.mkdir(path.dirname(outputFilePath), { recursive: true });
      await fs.promises.writeFile(outputFilePath, generatedAssets, "utf-8");

      console.log("Assets bundled to %s", outputFilePath);
    }
  };
}

export default defineConfig({
  base: "./",
  plugins: [react(), tsconfigPaths(), bundleWebAssets()]
});
