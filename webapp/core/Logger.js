(function() {
  "use strict";

  // Log library
  var winston = require("winston");
  var format = new Date().toLocaleTimeString();
  var path = require("path");
  var PROJECT_ROOT = path.join(__dirname, '..');
  var NodeFormat = require("util").format;

  /**
   * It defines a Winston Logger used in TerraMA²
   * @type {winston.Logger}
   */
  var logger = new winston.Logger({
    transports: [
      // performs output colorize in console
      new (winston.transports.Console)({
        timestamp: format,
        colorize: true,
        level: process.env.NODE_ENV === 'PRODUCTION' ? 'info' : 'debug', // defining debug to log console
        formatter: loggerFormat
      })
    ],
    exitOnError: false
  });

  // TODO: handle exception to a file?
  // logger.handleExceptions(new winston.transports.Console({ colorize: true, json: true, formatter: loggerFormat }));

  // keeping logger debug reference
  var oldDebug = logger.debug;
  // Decorator for logger info
  logger.debug = function() {
    return oldDebug.apply(this, formatLogArguments(arguments));
  };

  // keeping logger warning ref
  var oldWarning = logger.warn;
  // Decorator for logger warning
  logger.warn = function() {
    return oldWarning.apply(this, formatLogArguments(arguments));
  };

  // keeping logger error ref
  var oldError = logger.error;
  // Decorator for logger error
  logger.error = function() {
    return oldError.apply(this, formatLogArguments(arguments));
  };

  /**
   * It performs a Logger format stream.
   *
   * @param {any} args - A Winston Logger arguments
   * @param {string} args.level - A logger level
   * @param {string} args.timestamp - A logger timestamp
   * @param {string} args.message - A logger message
   * @returns {string} Formatted string to log
   */
  function loggerFormat(args) {
    return NodeFormat("[%s] %s %s", format, winston.config.colorize(args.level), args.message);
  }

  /**
   * Attempts to add file and line number info to the given log arguments.
   *
   * @param {...any} args - Logger arguments given
   * @return {any[]}
   */
  function formatLogArguments (args) {
    args = Array.prototype.slice.call(args);

    var stackInfo = getStackInfo(1);

    if (stackInfo) {
      // get file path relative to project root
      var calleeStr = '(' + stackInfo.relativePath + ':' + stackInfo.line + ')';

      if (typeof (args[0]) === 'string') {
        args[0] = calleeStr + ' ' + args[0];
      } else if (args[0] instanceof Error) {
        args[0] = calleeStr + ' ' + args[0].toString()
      } else if (args[0] instanceof Object) {
        args[0] = calleeStr + ' ' + JSON.stringify(args[0]);
      } else {
        args.unshift(calleeStr);
      }

      for(var i = 1; i < args.length; ++i) {
        if (args[0] instanceof Error) {
          args[i] = args[i].toString();
        }

        if (args[i] instanceof Object) {
          args[i] = JSON.stringify(args[i]);
        }
      }
    }

    return args;
  }

  /**
   * Parses and returns info about the call stack at the given index.
   *
   * @param {number} stackIndex - Stack trace index
   */
  function getStackInfo (stackIndex) {
    // get call stack, and analyze it
    // get all file, method, and line numbers
    var stacklist = (new Error()).stack.split('\n').slice(3)

    // stack trace format:
    // http://code.google.com/p/v8/wiki/JavaScriptStackTraceApi
    // do not remove the regex expresses to outside of this method (due to a BUG in node.js)
    var stackReg = /at\s+(.*)\s+\((.*):(\d*):(\d*)\)/gi;
    var stackReg2 = /at\s+()(.*):(\d*):(\d*)/gi;

    var s = stacklist[stackIndex] || stacklist[0];
    var sp = stackReg.exec(s) || stackReg2.exec(s);

    if (sp && sp.length === 5) {
      return {
        method: sp[1],
        relativePath: path.relative(PROJECT_ROOT, sp[2]),
        line: sp[3],
        pos: sp[4],
        file: path.basename(sp[2]),
        stack: stacklist.join('\n')
      };
    }
  }

  module.exports = logger;
} ());