const FTPClient = require('active-ftp');
const ftp = require("basic-ftp")
const { join } = require('path');
const logger = require('./../Logger');


/** Base class for representing FTP Error */
class FTPError extends Error { }

/** Exception used to describe that there is no active connection */
class NoSuchConnectionError extends Error {
  constructor() {
    super(`FTP is closed`);
  }
};

/** Exception used to describe that directory does not exists on Server. */
class DirectoryNotFoundError extends FTPError {
  constructor(directory, additionalMsg = '') {
    super(`The directory ${directory} not found on server. ${additionalMsg}`)
  }
}

// Helper
const abstractMethod = () => {
  throw new Error(`This method must be implemented`)
}

/**
 * Base class adapter for FTP Handling
 *
 * You must implement your own FTP module, such Active and Passive mode server.
 */
class FTPAdapter {
  constructor(config) {
    this._config = config;
  }

  /** Connects to FTP host */
  async connect() {
    abstractMethod();
  }

  /**
   * Disconnect from FTP host
   *
   * @abstract
   */
  async disconnect() {
    abstractMethod();
  }

  /** Checks for current opened session with FTP Host */
  isConnected() {
    return false;
  }

  /**
   * List files from provided path.
   *
   * @abstract
   * @param {string} path Path to list
   * @returns {Promise<string[]>}
   */
  async list(path) {
    abstractMethod();
  }

  /**
   * Change working directory to provided path
   *
   * @abstract
   * @param {string} path Base directory
   */
  async cd(path) {
    abstractMethod();
  }

  /**
   * Retrieves current directory
   *
   * @abstract
   * @returns {Promise<string>}
   */
  async pwd() {
    abstractMethod();
  }
}

/**
 * Implementation of FTP Server in Passive Mode
 *
 * It wraps internal FTP library in order to work with promises.
 *
 * **You should not use this interface directly**. Use @link Ftp class instead.
 */
class FTPPassiveAdapter extends FTPAdapter {
  /**
   * @param {Object} config Connection config
   */
  constructor(config) {
    super(config);
    this._config = config;
    this._client = new ftp.Client();
    this._client.ftp.verbose = true;
  }

  /** Connects to the FTP Passive Server */
  async connect() {
    const { _client } = this;

    await _client.access(this._config);
  }

  /** Disconnects from FTP Passive Server */
  async disconnect() {
    if (!this.isConnected()) {
      return;
    }

    await this._client.close();
  }

  isConnected() {
    return !this._client.closed;
  }

  async list(path) {
    try {
      if (path) {
        await this.cd(path);
      }
      const files = await this._client.list();
      return files.map(file => ({...file, type: (file.isDirectory ? 'd': 'f')}));
    } catch (err) {
      logger.debug(err);

      throw err;
    }

  }

  async cd(path) {
    const res = await this._client.cd(path);

    if (res.code !== 250) {
      throw new DirectoryNotFoundError(path, res.message);
    }
  }

  async pwd() {
    if (!this.isConnected()) {
      throw new NoSuchConnectionError();
    }

    return this._client.pwd();
  }
}

/**
 * Wrapper of FTP Client for ActiveFTP mode
 *
 * It aim's to override some of operations in order to work properly.
 *
 * Whenever a command sent, it opens a connection with FTP Host, send command and then
 * close connection.
 *
 * @todo Remove the dependency of FTPClient (Active) mode and implement
 * own module in order to scale appication
 */
class InternalActiveFTP extends FTPClient {
  /**
   * Wraps internal command to send IP address.
   *
   * Since Active mode requires IP address, you can get local address information
   * with connected socket. In this way, we are overriding the method
   * **sendPort** in order to retrieves localIpaddress and attach it
   * to config property
   */
  _sendPort() {
    this._config.client = this._cmdSocket.localAddress;
    super._sendPort();
  }

  /** Internal pwd command in order to retrieve current directory properly */
  _pwd(callback) {
    this._callback = res => {
      callback(res);
    }
    this._buffers = []

    this._cmdSocket.once('data', (data) => {
      var responses = data.toString().split('\r\n')
      for (var response of responses) {
        const trimmedResponse = response.toString().trim();
        if (trimmedResponse.length) {
          const responseFragments = response.split(" ");
          const code = parseInt(responseFragments[0]);
          const currentDir = responseFragments[1].replace(/\"/g, "");

          if (code === 257) {
            callback(undefined, currentDir);
            // return resolve(currentDir);
          } else
            this._codeError(code, "Expected 257")
            // return reject(new FTPError(`Could not get current dir on server.`));
        }
      }
    });

    this._sendCommand("PWD")
  }

  /** Retrieves current directory on remote FTP server */
  pwd() {
    return new Promise((resolve, reject) => {
      const noop = (err, dir) => {
        if (err)
          return reject(err);
        return resolve(dir);
      }

      this._performPublicAction(noop, async (internal) => {
        this._pwd((err, dir) => {
          internal(dir);
        });
      })
    })
  }

  /**
   * List directories from provided directory
   *
   * @param {*} folder Path to filter
   * @returns {string[]} File list (Not formated)
   */
  list(folder) {
    return new Promise(async (resolve, reject) => {
      const callback = (error, list) => {
        if (error)
          return reject(error);
        return resolve(list)
      }

      const destDir = join(await this.pwd(), folder);

      this._performPublicAction(callback, async innerCallback => {
        this._list(destDir, list => {
          var files = list.split('\r\n');
          files.pop()
          innerCallback(files)
        })
      })
    })
  }
}

/**
 * Implementation of FTP Server in Active Mode
 *
 * It wraps internal FTP library in order to work with promises.
 *
 * **You should not use this interface directly**. Use @link Ftp class instead.
 */
class FTPActiveAdapter extends FTPAdapter {
  constructor(config) {
    super(config);

    // Override config password property
    config.pass = config.password;
    this._client = new InternalActiveFTP(config);
  }

  async connect() {
    const fakeClient = new ftp.Client();

    await fakeClient.access(this._config);
    await fakeClient.close();

    return;
  }

  async disconnect() {
    return;
  }

  async cd(path) {
    await this.list(path);
    this.currentDir = path;
  }

  async pwd() {
    return this._client.pwd();
  }

  async list(path) {
    return await this._client.list(`${path}`);
  }
}

/**
 * Implementation of FTP library to work with both Active and Passive mode.
 *
 * You may use the flag **mode** to **active** if would like to work with Active Mode handler
 *
 * @example
 *
 * const config = {
 *   host: 'localhost',
 *   port: 21,
 *   user: 'anonymous',
 *   password: '1234',
 *   mode: 'active'
 * };
 *
 * const ftp = new Ftp(config);
 *
 * try {
 *   await ftp.connect();
 *
 *   console.log(`Files on root folder ${await ftp.list()}`);
 *
 *   await ftp.cd('someDir');
 *
 *   console.log(`Current dir ${await ftp.pwd()}`)
 * } catch (err) {
 *  // Handle error
 * }
 */
class Ftp {
  /**
   * @param {Object} config Connection config
   * @param {string} config.host FTP Hosts
   * @param {number} config.port FTP Port number
   * @param {string} config.user FTP user
   * @param {string} config.password FTP user password
   * @param {string?} config.mode Changes to active/passive mode. Default is 'passive'.
   * @param {number?} config.activePort Port number to Active mode. Used only in 'active' mode.
   */
  constructor(config) {
    /**
     * Responsible adapter of FTP library
     * @type {FTPAdapter}
     */
    this._adapter = null;
    this._activeMode = false;

    this._detectMode(config);
  }

  /**
   * Set current FTP mode adapter
   * @private
   * @param {Object} config connection config
   */
  _detectMode(config) {
    if (config.mode === 'active') {
      this._activeMode = true;
      this._activePort = config.activePort || 9999;

      this._adapter = new FTPActiveAdapter(config);
    } else {
      this._adapter = new FTPPassiveAdapter(config);
    }
  }

  /**
   * Connects to FTP server
   */
  async connect() {
    await this._adapter.connect();
  }

  /** Disconnects from FTP Server */
  async disconnect() {
    await this._adapter.disconnect();
  }

  isConnected() {
    return this._adapter.isConnected();
  }

  /**
   * List files from provided directory.
   *
   * It assumes that you are listing directory over home dir, using @link Ftp.pwd()
   *
   * @param {string} path base path to list
   */
  async list(path) {
    return await this._adapter.list(path);
  }

  /**
   * Change working directory
   * @param {string} path Directory path
   */
  async cd(path) {
    await this._adapter.cd(path);
  }

  /** Retrieves current working directory */
  async pwd() {
    return this._adapter.pwd();
  }
}

module.exports = Ftp;