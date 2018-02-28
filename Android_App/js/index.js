var app = {
    macAddress: "20:15:05:05:61:99",  // get your mac address from bluetoothSerial.list
    chars: "",


/*
    Application Constructor
 */
    initialize: function() {
        this.bindEvents(); //Add action listener
        console.log("Starting the app");
    },
/*
    bind any events that are required on startup to listeners:
    document.addEventListener(event, function, useCapture)
    {
    event: Required. A String that specifies the name of the event.
    function: Required. Specifies the function to run when the event occurs. 
    useCapture: Optional. A Boolean value that specifies whether the event should be executed in the capturing or in the bubbling phase.
    }
*/
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
        connectButton.addEventListener('touchend', app.manageConnection, false);
        turnleft.addEventListener('touchstart',app.turnleft, false);
        turnright.addEventListener('touchstart',app.turnright, false);
        turnleft.addEventListener('touchend',app.backmiddle, false);
        turnright.addEventListener('touchend',app.backmiddle, false);
        degree0.addEventListener('touchstart',app.zero, false);
        degree45.addEventListener('touchstart',app.fourtyfive, false);
        degree90.addEventListener('touchstart',app.ninty, false);
        degree135.addEventListener('touchstart',app.onethirtyfive, false);
        degree180.addEventListener('touchstart',app.oneeighty, false);
        degree225.addEventListener('touchstart',app.twotwentyfive, false);
        degree270.addEventListener('touchstart',app.twoseventy, false);
        degree315.addEventListener('touchstart',app.threefifteen, false);
        degree0.addEventListener('touchend',app.stop, false);
        degree45.addEventListener('touchend',app.stop, false);
        degree90.addEventListener('touchend',app.stop, false);
        degree135.addEventListener('touchend',app.stop, false);
        degree180.addEventListener('touchend',app.stop, false);
        degree225.addEventListener('touchend',app.stop, false);
        degree270.addEventListener('touchend',app.stop, false);
        degree315.addEventListener('touchend',app.stop, false);  
    },

/*
    this runs when the device is ready for user interaction:
*/
    onDeviceReady: function() {
        // check to see if Bluetooth is turned on.
        // this function is called only
        //if isEnabled(), below, returns success:
        var listPorts = function() {
            // list the available BT ports:
            bluetoothSerial.list(
                function(results) {
                    app.display(JSON.stringify(results));
                },
                function(error) {
                    app.display(JSON.stringify(error));
                }
            );
        }

        // if isEnabled returns failure, this function is called:
        var notEnabled = function() {
            app.display("Bluetooth is not enabled.")
        }

         // check if Bluetooth is on:
        bluetoothSerial.isEnabled(
            listPorts,
            notEnabled
        );
    },
/*
    Connects if not connected, and disconnects if connected:
*/
    manageConnection: function() {

        // connect() will get called only if isConnected() (below)
        // returns failure. In other words, if not connected, then connect:
        var connect = function () {
            // if not connected, do this:
            // clear the screen and display an attempt to connect
            app.clear();
            app.display("Attempting to connect. " +
                "Make sure the serial port is open on the target device.");
            // attempt to connect:
            bluetoothSerial.connect(
                app.macAddress,  // device to connect to
                app.openPort,    // start listening if you succeed
                app.showError    // show the error if you fail 
            );
        };

        // disconnect() will get called only if isConnected() (below)
        // returns success  In other words, if  connected, then disconnect:
        var disconnect = function () {
            app.display("attempting to disconnect");
            // if connected, do this:
            bluetoothSerial.disconnect(
                app.closePort,     // stop listening to the port
                app.showError      // show the error if you fail
            );
        };

        // here's the real action of the manageConnection function:
        bluetoothSerial.isConnected(disconnect, connect);
    },
/*
    subscribes to a Bluetooth serial listener for newline
    and changes the button:
*/
    openPort: function() {
        // if you get a good Bluetooth serial connection:
        app.display("Connected to: " + app.macAddress);
        // change the button's name:
        connectButton.innerHTML = "Disconnect";
        // set up a listener to listen for newlines
        // and display any new data that's come in since
        // the last newline:
        bluetoothSerial.subscribe('\n', function (data) {
            app.clear();
            app.display(data);
        });
    },

/*
    unsubscribes from any Bluetooth serial listener and changes the button:
*/
    closePort: function() {
        // if you get a good Bluetooth serial connection:
        app.display("Disconnected from: " + app.macAddress);
        // change the button's name:
        connectButton.innerHTML = "Connect";
        // unsubscribe from listening:
        bluetoothSerial.unsubscribe(
                function (data) {
                    app.display(data);
                },
                app.showError
        );
    },
/*
    appends @error to the message div:
*/
    showError: function(error) {
        app.display(error);
    },

/*
    appends @message to the message div:
*/
    display: function(message) {
        var display = document.getElementById("message"), // the message div
            lineBreak = document.createElement("br"),     // a line break
            label = document.createTextNode(message);     // create the label

        display.appendChild(lineBreak);          // add a line break
        display.appendChild(label);              // add the message node
    },
/*
    clears the message div:
*/
    clear: function() {
        var display = document.getElementById("message");
        display.innerHTML = "";
    },
    
    
    turnleft: function() {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0x64;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
    turnright: function() {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0x64;
        data[3] = 0x02;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
    backmiddle: function() {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0x64;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    
    },
  
      ninty :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0xC8;
        data[2] = 0x64;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
    fourtyfive :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0xC8;
        data[2] = 0xC8;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
   
        zero :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0xC8;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
        threefifteen :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x00;
        data[2] = 0xC8;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
        twoseventy :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x00;
        data[2] = 0x64;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
        twotwentyfive :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
        oneeighty :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0x00;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
   
    onethirtyfive :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0xC8;
        data[2] = 0x00;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    },
    
    stop :function()    {
        var data = new Uint8Array(10);
        data[0] = 0xF0;
        data[1] = 0x64;
        data[2] = 0x64;
        data[3] = 0x01;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0xFF;
        bluetoothSerial.write(data);
    }
};      // end of app

