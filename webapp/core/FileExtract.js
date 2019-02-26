// External dependencies
const Unzip = require('adm-zip');
const { tmpdir } = require('os');

const fileExtractModule = {
  /**
   * Unzip files to directory
   *
   * @param {string} filePath Path to the zip file to extract
   * @param {*} temporaryDir Directory where files will be extracted. Default os.tmpdir()
   * @returns {any[]} List of files extracted
   */
  unzip(filePath, temporaryDir) {
    const zipFile = new Unzip(filePath);

    if (!temporaryDir) {
      temporaryDir = tmpdir();
    }

    // Extract all files to folterPath
    zipFile.extractAllTo(temporaryDir, /*overwrite*/ true);

    return zipFile;
  }
};


module.exports = fileExtractModule;