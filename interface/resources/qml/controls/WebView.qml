import QtQuick 2.5
import QtWebEngine 1.1
import QtWebChannel 1.0
import "../controls-uit" as HiFiControls
import FileTypeProfile 1.0

Item {
    property alias url: root.url
    property alias scriptURL: root.userScriptUrl
    property alias eventBridge: eventBridgeWrapper.eventBridge
    property alias canGoBack: root.canGoBack;
    property var goBack: root.goBack;
    property bool keyboardEnabled: true  // FIXME - Keyboard HMD only: Default to false
    property bool keyboardRaised: false
    property bool punctuationMode: false

    // FIXME - Keyboard HMD only: Make Interface either set keyboardRaised property directly in OffscreenQmlSurface
    // or provide HMDinfo object to QML in RenderableWebEntityItem and do the following.
    /*
    onKeyboardRaisedChanged: {
        keyboardEnabled = HMDinfo.active;
    }
    */

    QtObject {
        id: eventBridgeWrapper
        WebChannel.id: "eventBridgeWrapper"
        property var eventBridge;
    }

    WebEngineView {
        id: root
        objectName: "webEngineView"
        x: 0
        y: 0
        width: parent.width
        height: keyboardEnabled && keyboardRaised ? parent.height - keyboard.height : parent.height

        profile: FileTypeProfile {
            id: webviewProfile
            storageName: "qmlWebEngine"
        }

        property string userScriptUrl: ""

        // creates a global EventBridge object.
        WebEngineScript {
            id: createGlobalEventBridge
            sourceCode: eventBridgeJavaScriptToInject
            injectionPoint: WebEngineScript.DocumentCreation
            worldId: WebEngineScript.MainWorld
        }

        // detects when to raise and lower virtual keyboard
        WebEngineScript {
            id: raiseAndLowerKeyboard
            injectionPoint: WebEngineScript.Deferred
            sourceUrl: resourceDirectoryUrl + "/html/raiseAndLowerKeyboard.js"
            worldId: WebEngineScript.MainWorld
        }

        // User script.
        WebEngineScript {
            id: userScript
            sourceUrl: root.userScriptUrl
            injectionPoint: WebEngineScript.DocumentReady  // DOM ready but page load may not be finished.
            worldId: WebEngineScript.MainWorld
        }

        userScripts: [ createGlobalEventBridge, raiseAndLowerKeyboard, userScript ]

        property string newUrl: ""

        webChannel.registeredObjects: [eventBridgeWrapper]

        Component.onCompleted: {
            // Ensure the JS from the web-engine makes it to our logging
            root.javaScriptConsoleMessage.connect(function(level, message, lineNumber, sourceID) {
                console.log("Web Entity JS message: " + sourceID + " " + lineNumber + " " +  message);
            });

            root.profile.httpUserAgent = "Mozilla/5.0 Chrome (HighFidelityInterface)";
        }

        onFeaturePermissionRequested: {
            grantFeaturePermission(securityOrigin, feature, true);
        }

        onLoadingChanged: {
            keyboardRaised = false;
            punctuationMode = false;
            keyboard.resetShiftMode(false);

            // Required to support clicking on "hifi://" links
            if (WebEngineView.LoadStartedStatus == loadRequest.status) {
                var url = loadRequest.url.toString();
                if (urlHandler.canHandleUrl(url)) {
                    if (urlHandler.handleUrl(url)) {
                        root.stop();
                    }
                }
            }
        }

        onNewViewRequested:{
            // desktop is not defined for web-entities or tablet
            if (typeof desktop !== "undefined") {
                desktop.openBrowserWindow(request, profile);
            } else {
                console.log("onNewViewRequested: desktop not defined");
            }
        }
    }

    HiFiControls.Keyboard {
        id: keyboard
        raised: parent.keyboardEnabled && parent.keyboardRaised
        numeric: parent.punctuationMode
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

}
