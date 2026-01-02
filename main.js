const {BrowserWindow,app} = require('electron')

function createBrowser(){
    const browser = new BrowserWindow({
        width:800,
        height:600
    })

    return browser
}

app.on('ready',(e)=>{
    let browser = createBrowser();
    browser.loadFile('relicUpgradeGUI.html')
})