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

    var hasError = false;
    var pythonCheckCommandResult = '';
    var pythonCheckCommandError = '';

    try {
      pythonCheckCommandResult = memberExecSync(pythonCheckCommand, { encoding: 'utf8' });
    } catch(e) {
      if(e.stderr === '') {
        pythonCheckCommandResult = e.stdout;
      } else {
        hasError = true;
        pythonCheckCommandResult = '';
        pythonCheckCommandError = e.stderr;
      }
    }

    memberFs.unlink(pythonFilePath);

    var textLines = pythonCheckCommandResult.split(/\r?\n/);
    var textLinesLength = textLines.length;

    var messages = '';
    var hasPythonError = false;

    if(!hasError && pythonCheckCommandResult !== '' && textLinesLength > 0) {
      for(var i = 0; i < textLinesLength; i++) {
        if(i > 0 && textLines[i] !== '') {
          messages += textLines[i] + '\n';

          if(textLines[i].substring(0, 1) === 'E')
            hasPythonError = true;
        }
      }
    } else {
      if(hasError) {
        messages += 'Attention! Occurred a system error, contact the administrator.\n\nError:';
      } else {
        messages += 'No errors found.';
      }
    }

    var returnObject = {
      hasError: hasError,
      systemError: pythonCheckCommandError.replace(/\r\n/, '\n'),
      hasPythonError: hasPythonError,
      messages: messages
    };

    return returnObject;
  };
};

module.exports = ScriptChecker;
