/**
 * This file defines TerraMA2 signals for handling TCP communication.
 * 
 * @readonly
 * @enum {number}
 */
var Signals = module.exports = {
  /**
   * Stop service signal
   */
  TERMINATE_SERVICE_SIGNAL: 0,
  /**
   * Check if the connection is alive and get service information
   */
  STATUS_SIGNAL: 1,
  /**
   * Data signal, followed by data
   */
  ADD_DATA_SIGNAL: 2,
  /**
   * Start signal, queue the collection, analysis, etc
   */
  START_PROCESS_SIGNAL: 3,
  /**
   * Log signal, send error message to node.js server
   */
  LOG_SIGNAL: 4,
  /**
   * Used for remove data from TcpServices
   */
  REMOVE_DATA_SIGNAL: 5,
  /**
   * Used to notify process has been finalized
   */
  PROCESS_FINISHED_SIGNAL: 6,
  /**
   * Used to update service data. It is mandatory when a service has been initialized.
   */
  UPDATE_SERVICE_SIGNAL: 7,
  /**
   * Used to validate a TerraMA² process
   */
  VALIDATE_PROCESS_SIGNAL: 8
};
