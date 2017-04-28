#!/usr/bin/env node

/**
 * TerraMA² Logger Application
 * @type {winston.Logger}
 */
var logger = require("./../core/Logger");
/**
 * TerraMA² Application settings
 * @type {Application}
 */
var Application = require("../core/Application");

/**
 * Handler for closing connection. It just displays logger output
 * 
 * @returns {void}
 */
function onSocketDisconnected() {
  logger.info("Close");
  process.exit(0);
}

/**
 * It handles connection timeout
 */
function onTimeoutOccurred() {
  logger.error("Timeout exceeded");
  process.exit(1);
}

/**
 * It handles connection error performing logging exception and finalize script with error.
 * 
 * @param {Error} err - Socket error
 */
function onConnectError(err) {
  logger.error("Could not connect due " + err.toString());
  process.exit(1);
}

try {
  Application.load();

  var config = Application.getContextConfig();
  var protocol = config.ssl ? "https://" : "http://";
  var connURL = protocol + "0.0.0.0:" + config.port;

  /**
   * Socket options
   * 
   * @type {any}
   */
  var options = {
    reconnectionAttempts: 3, // only three times
    timeout: 10000 // 10 seconds
  };
  logger.info("Connecting to " + connURL);

  var socket = require("socket.io-client")(connURL, options);
  socket.on("connect", function handleConnected() {
    logger.info("connected, sending STOP_ALL");

    socket.emit("stopAll");
    setTimeout(function() {
      logger.info("Done");
      process.exit(0);
    }, 2000);
  });

  socket.on("connect_timeout", onTimeoutOccurred);
  socket.on("disconnect", onSocketDisconnected);
  socket.on("connect_error", onConnectError);
} catch (e) {
  logger.error("Could not loading TerraMA² configuration due " + e.stack);
  process.exit(1);
}