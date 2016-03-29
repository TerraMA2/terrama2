/*
  This file defines TerraMA2 signals for handling TCP communication.
*/

var Signals = module.exports = {
  TerminateSignal: 0, // Stop service signal
  StatusSignal: 1, // Check if the connection is alive and get service information
  DataSignal: 2, // Data signal, followed by data
  StartSignal: 3, // Start signal, queue the collection, analysis, etc
  ErrorSignal: 4 // Error signal, send error message to node.js server
};
