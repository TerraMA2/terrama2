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

    var buffer = require('crypto').randomBytes(24);
    var pythonFilePath = path.join(__dirname, '../tmp/python-' + buffer.toString('hex') + '.py');
    var pylintConfFilePath = path.join(__dirname, '../config/pylintrc');
    var pythonCheckCommand = "C:\\Python27\\Scripts\\pylinte.exe --rcfile=" + pylintConfFilePath + " " + pythonFilePath;

    try {
      memberFs.writeFileSync(pythonFilePath, script);

      var pythonCheckCommandResult = memberExecSync(pythonCheckCommand, { encoding: 'utf8' });

      console.log("----------------------------------------");
      console.log(pythonCheckCommandResult);
      console.log("----------------------------------------");
    } catch(e) {
      if(e.stderr === '') {
        console.error(e.stdout);
      } else {
        console.error(e.stderr);
      }
    }

    memberFs.unlink(pythonFilePath);
  };
};

module.exports = ScriptChecker;
