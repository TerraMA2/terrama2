// NodeJS Dependencies
const child_process = require('child_process');
const fs = require('fs');
const os = require('os');
const path = require('path');

// TerraMA2 Dependencies
const Exportation = new (require('./Exportation.js'))();
const FileExtract = require('./FileExtract');
const Utils = require('./Utils.js');
const DataManager = require('./DataManager');

/**
 * Execute a command in terminal within child process
 *
 * @param {string} command - Command string to execute
 * @returns {Promise<void>}
 */
function execute(command) {
  return new Promise((resolve, reject) => {
    child_process.exec(command, { maxBuffer: 20 * 1024 * 1024 }, (commandErr, ) => (
      commandErr ? reject(commandErr) : resolve()
    ));
  });
}

/**
 * Used when no shape file found in uncompressed data
 */
class NoShapeFileFoundError extends Error { }

/**
 * Used when multiple shape files found in uncompressed data
 */
class MultipleShapeFileFoundError extends Error { }


/**
 * @class ShapeImporter
 *
 * ShapeImporter represents an abstraction of tool shp2pgsql.
 * You can define a temporary directory to unpack compressed files (zip,bz2,gz).
 * Once uncompressed, you can upload to both database or local directory using
 * TerraMA² DataProvider structure.
 *
 * @example
 *
 * const importer = new ShapeImporter('/tmp', 4326, 'LATIN1');
 * // Import shapefiles to DataProvider 1
 * importer.unzip('/path/to/shape_zip.zip').toDatabase('tableShape1', 1);
 *
 * // Import shapefiles to the folder of DataProvider 2
 * importer.toDataProvider(2, 'myshape_file_unzipped.shp');
 */
class ShapeImporter {
  /**
   * Builds a new shape importer
   *
   * @param {string} temporaryDir Temporary directory to store files
   * @param {number} srid Spatial Reference System
   * @param {string} encoding Data encoding (LATIN1, UTF-8, etc)
   */
  constructor(temporaryDir, srid, encoding) {
    /**
     * Temporary directory where files stored
     * @type {string}*
     * @default os.tmpdir()
     */
    this.temporaryDir = temporaryDir || os.tmpdir();
    /**
     * Spatial Reference System
     * @type {number}
     */
    this.srid = srid;
    /**
     * Enconding of data to import
     * @type {string}
     */
    this.encoding = encoding;
    /**
     * List of files associated with Shape (.shx, prj, etc).
     * Remember that it will only import files with extension .shp.
     * @type {string[]}
     */
    this.files = [];
    /**
     * Path to the shape file
     * @type {string}
     */
    this.shapeFile = null;
  }

  /**
   * Tries to unzip the provided file in temporary directory.
   * Once extracted, checks if exists any shapefile.
   *
   * @throws {Error} when no shapefile or more than one found
   * @throws {any} when could not extract file as zip
   *
   * @param {string} filePath Path to the zip file
   * @param {*} targetDir Path to extract. Default is temporary dir
   * @returns {ShapeImporter} ShapeImporter instance
   */
  unzip(filePath, targetDir) {
    let numberOfShapeFiles = 0;

    if (targetDir) {
      this.temporaryDir = targetDir;
    }

    this.files = FileExtract.unzip(filePath, this.temporaryDir).getEntries().map(entry => {
      if (entry.entryName.endsWith('.shp')) {
        ++numberOfShapeFiles;
        this.shapeFile = path.join(this.temporaryDir, entry.entryName);
      }
      return entry.entryName;
    });

    if (numberOfShapeFiles === 0) {
      throw new NoShapeFileFoundError(`No shapefile found!`);
    }

    if (numberOfShapeFiles > 1) {
      throw new MultipleShapeFileFoundError(`More than one shapefile found!`);
    }

    return this;
  }

  /**
   * Retrieves a list of files loaded in instance
   *
   * @returns {string[]} Files in current instance
   */
  getFiles() {
    return this.files;
  }

  /**
   * Clean the temporary directory
   */
  clean() {
    if(this.temporaryDir !== null) {
      Utils.deleteFolderRecursively(this.temporaryDir, () => {});
    }

    Exportation.deleteInvalidFolders();
  }

  /**
   * Import the loaded files to database
   *
   * @param {string} tableName - Table name to Import
   * @param {number} dataProviderId - Data Provider Identifier which contains PostgreSQL connection parameters
   * @returns {Promise<boolean>}
   */
  toDatabase(tableName, dataProviderId) {
    return new Promise(async (resolve, reject) => {
      try {
        // Retrieves psql syntax
        const connectionString = await Exportation.getPsqlString(dataProviderId);

        // Checks if table exists before upload
        let tableFound = await Exportation.tableExists(tableName, dataProviderId);

        if(tableFound) {
          throw new Error("Table already exists!");
        }

        // Build shp2pgsql command
        const commandStr = `${connectionString.exportPassword} ${Exportation.shp2pgsql()} -I -s ${this.srid} -W " ${this.encoding}" ${this.shapeFile} ${tableName} | ${connectionString.connectionString}`;

        // Once command built, tries to execute shp2pgsql import
        await execute(commandStr);

        // Ensure that operation has been succeeded
        tableFound = await Exportation.tableExists(tableName, dataProviderId);

        if (tableFound) {
          return resolve();
        }

        throw new Error('Unknown error: Make sure you have shp2pgsql installed');
      } catch (err) {
        return reject(err);
      } finally {
        // Clean uploaded files
        this.clean();
      }
    });
  }

  /**
   * Import the loaded files to data provider (type file) directory
   *
   * @todo Remove Promise statement, since it does not uses async operation
   * @param {number} id DataProvider Identifier
   * @param {*} mask Mask name for the shapefile
   * @returns {Promise<void>}
   */
  toDataProvider(id, mask) {
    return new Promise(async (resolve, reject) => {
      try {
        const dataProvider = await DataManager.getDataProvider({ id });
        const dataProviderPath = dataProvider.uri.replace("file://", "");

        if(!fs.existsSync(dataProviderPath)) {
          return reject(new Error("Invalid data provider path!"));
        }

        const finalFilePath = (dataProviderPath + "/" + mask).split("//").join("/");

        if(!fs.existsSync(finalFilePath)) {
          const maskArray = mask.split("/");

          let pathCreationResult = null;

          if(maskArray.length > 1) {
            pathCreationResult = Exportation.createPathToFile(dataProviderPath, maskArray);
          } else {
            pathCreationResult = {
              error: null,
              createdPath: dataProviderPath
            };
          }

          if(!pathCreationResult.error) {
            const createdPath = pathCreationResult.createdPath;
            const newFilename = maskArray[maskArray.length - 1].replace(path.extname(maskArray[maskArray.length - 1]), "");

            Exportation.copyShpFiles(this.temporaryDir, createdPath, newFilename);

            return resolve();
          } else {
            throw pathCreationResult.error;
          }
        } else {
          return reject(new Error("File already exists!"));
        }
      } catch (err) {
        return reject(err);
      } finally {
        this.clean();
      }
    });
  }
}

module.exports = {
  MultipleShapeFileFoundError,
  NoShapeFileFoundError,
  ShapeImporter
};