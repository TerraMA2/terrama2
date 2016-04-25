/*
  This file defines TerraMA2 signals for handling TCP communication.
*/

var Signals = module.exports = {
  TERMINATE_SERVICE_SIGNAL: 0, // Stop service signal
  STATUS_SIGNAL: 1, // Check if the connection is alive and get service information
  ADD_DATA_SIGNAL: 2, // Data signal, followed by data
  START_PROCESS_SIGNAL: 3, // Start signal, queue the collection, analysis, etc
  LOG_SIGNAL: 4, // Log signal, send error message to node.js server
  REMOVE_DATA_SIGNAL: 5,
  PROCESS_FINISHED_SIGNAL: 6
};
