declare global {
  interface WebViewMessageEvent extends MessageEvent {
    additionalObjects: ArrayLike<FileSystemFileHandle | FileSystemDirectoryHandle | null>;
    source: WebView;
  }

  interface SharedBufferReceivedEvent extends Event {
    additionalData: any | undefined;
    source: WebView;

    getBuffer(): ArrayBuffer;
  }

  interface WebView {
    addEventListener(type: "message", listener: (event: WebViewMessageEvent) => void): void;
    addEventListener(type: "sharedbufferreceived", listener: (event: SharedBufferReceivedEvent) => void): void;
    removeEventListener(type: "message", listener: (event: WebViewMessageEvent) => void);
    removeEventListener(type: "sharedbufferreceived", listener: (event: SharedBufferReceivedEvent) => void): void;
    postMessage(message: any): void;
    postMessageWithAdditionalObjects(message: any, additionalObjects: ArrayLike<File>): void;
    releaseBuffer(buffer: ArrayBuffer): void;
  }

  interface Chrome {
    webview: WebView;
  }
}

export {};
