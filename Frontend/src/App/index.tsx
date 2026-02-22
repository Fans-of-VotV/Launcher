import backgroundImg from "src/media/images/background.png";

import styles from "./styles.module.css";

export default function App() {
  const onPlayClicked = () => {
    webview.postMessage({ type: "play" });
  };

  return (<>
    <img
      className={styles.background_image}
      src={backgroundImg} />

    <div className={styles.content}>
      <h1 className={styles.title}>
        VOICES OF THE VOID
      </h1>

      <div className={styles.main_wrapper}>
      <main className={styles.main}>
        <button className={styles.play_button} onClick={onPlayClicked}>
          ИГРАТЬ
        </button>
      </main>
      </div>
    </div>
  </>);
}
