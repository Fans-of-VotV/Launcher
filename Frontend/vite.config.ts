import fs from "node:fs";
import path from "node:path";
import mime from "mime-types";
import { defineConfig } from "vite";
import type { Plugin } from "vite";
import react from "@vitejs/plugin-react";

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

      const templateFilePath = path.resolve(__dirname, "../Source/WebAssets.cpp.in");
      const outputFilePath = path.resolve(__dirname, "../Source/Generated/WebAssets.cpp");

      const templateFile = await fs.promises.readFile(templateFilePath, "utf-8");

      const assetTemplateBeginMarker = "@ASSET_TEMPLATE_BEGIN@";
      const assetTemplateEndMarker = "@ASSET_TEMPLATE_END@";

      const assetTemplateBegin = templateFile.search(assetTemplateBeginMarker);
      const assetTemplateEnd = templateFile.search(assetTemplateEndMarker);
      const assetTemplate = templateFile.substring(assetTemplateBegin + assetTemplateBeginMarker.length, assetTemplateEnd);

      let generatedAssets = "";
      
      let assetId = 0;
      for (const assetPath of files) {
        const buffer = await fs.promises.readFile(assetPath);
        const bytes = Array.from(buffer)
          .map(b => `0x${b.toString(16).padStart(2, "0")}`)
          .join(',');

        const pathname = '/' + path.relative(distDir, assetPath).replaceAll('\\', '/');
        const mimeType = mime.lookup(pathname) || "application/octet-stream";

        const generated = assetTemplate
          .replaceAll("@ASSET_ID@", String(assetId))
          .replaceAll("@ASSET_BYTES@", bytes)
          .replaceAll("@ASSET_PATHNAME@", pathname)
          .replaceAll("@ASSET_MIME_TYPE@", mimeType);

        generatedAssets += generated + '\n';

        ++assetId;
      }

      const result =
        templateFile.substring(0, assetTemplateBegin)
        + generatedAssets
        + templateFile.substring(assetTemplateEnd + assetTemplateEndMarker.length);

      await fs.promises.mkdir(path.dirname(outputFilePath), { recursive: true });
      await fs.promises.writeFile(outputFilePath, result, "utf-8");

      console.log("Assets bundled to %s", outputFilePath);
    }
  };
}

export default defineConfig({
  base: "./",
  plugins: [react(), bundleWebAssets()]
});
