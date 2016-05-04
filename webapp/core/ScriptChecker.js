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
    var pythonCheckCommand = "pylint --rcfile=" + pylintConfFilePath + " " + pythonFilePath;

    memberFs.writeFileSync(pythonFilePath, script);

    var pythonCheckCommandResult = '';

    try {
      pythonCheckCommandResult = memberExecSync(pythonCheckCommand, { encoding: 'utf8' });
    } catch(e) {
      if(e.stderr === '') {
        pythonCheckCommandResult = e.stdout;
      } else {
        pythonCheckCommandResult = '';
        console.log(e.stderr);
      }
    }

    memberFs.unlink(pythonFilePath);

    var textLines = pythonCheckCommandResult.split(/\r?\n/);
    var textLinesLength = textLines.length;

    var messages = '';
    var hasError = false;

    if(textLinesLength > 0) {
      for(var i = 0; i < textLinesLength; i++) {
        if(i > 0 && textLines[i] !== '') {
          messages += textLines[i] + '<br/>';

          if(textLines[i].substring(0, 1) === 'E')
            hasError = true;
        }
      }
    }

    var returnObject = {
      hasError: hasError,
      messages: messages
    };

    return returnObject;
  };
};

module.exports = ScriptChecker;
