// requires electron
const { app, BrowserWindow } = require("electron");
const Is_Mac = process.platform === "darwin";

function Create_Window() 
{
  const win = new BrowserWindow({
    title: "Evergreen Chess",
    width: 800,
    height: 600,
  });

  win.loadFile("index.html");
  return win; 
}

// Run the application
app.whenReady().then(() => {
  Create_Window();

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0){ 
      Create_Window();
    }
  })
})

app.on("window-all-closed", () => {
  if (!Is_Mac) app.quit();
})
