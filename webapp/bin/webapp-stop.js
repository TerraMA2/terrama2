#!/usr/bin/env node

var logger = require("./../core/Logger");
var Application = require("../core/Application");

function closeSocket() {
  logger.info("Close");
}

try {
  Application.load();

  var config = Application.getContextConfig();
  var protocol = config.ssl ? "https://" : "http://";
  logger.info(protocol + "0.0.0.0:" + config.port);

  var socket = require("socket.io-client")(protocol + "0.0.0.0:" + config.port);
  socket.on("connect", function handleConnected() {
    console.log("connected, sending STOP_ALL");

    socket.emit("stopAll");
    setTimeout(function() {
      process.exit(0);
    }, 2000);
  });
  socket.on("disconnect", closeSocket);
} catch (e) {
  logger.error("Could not loading TerraMA² configuration due " + e.stack);
  process.exit(1);
}