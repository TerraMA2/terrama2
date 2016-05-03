"use strict";

/**
 * Class responsible for checking the analysis script.
 * @class ScriptChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberFs - 'fs' module.
 * @property {function} memberExecSync - ExecSync function.
 */
var ScriptChecker = function() {

  // 'fs' module
  var memberFs = require('fs');
  // ExecSync function
  var memberExecSync = require('child_process').execSync;

  /**
   * Checks the received Python script.
   * @param {string} script - Python script
   *
   * @function checkPythonScript
   * @memberof ScriptChecker
   * @inner
   */
  this.checkPythonScript = function(script) {
    var path = require('path');

    try {
      var buffer = require('crypto').randomBytes(24);
      var pythonFilePath = path.join(__dirname, '../tmp/python-' + buffer.toString('hex') + '.py');
      var pylintConfFilePath = path.join(__dirname, '../config/pylintrc');

      memberFs.writeFileSync(pythonFilePath, script);

      var pythonCheckCommand = "C:\\Python27\\Scripts\\pylinte.exe --rcfile=" + pylintConfFilePath + " " + pythonFilePath;

      var pythonCheckCommandResult = memberExecSync(pythonCheckCommand, { encoding: 'utf8' });

      console.log("----------------------------------------");
      console.log(pythonCheckCommandResult);
      console.log("----------------------------------------");

      memberFs.unlink(pythonFilePath);
    } catch(e) {
      if(e.stderr === '') {
        console.error(e.stdout);
      } else {
        console.error(e.stderr);
      }
    }
  };
};

module.exports = ScriptChecker;
