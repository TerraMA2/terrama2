const url = require('url');
const path = require('path');

module.exports = {
  /**
   * Joins the URL with given path
   *
   * @note
   * The native nodejs `url.resolve` does not handle proper the path ending without slash '/' on given url.
   * For example:
   * url.resolve('http://terrama2_webapp_1:36000/mpmt', 'mypath') outputs `http://terrama2_webapp_1:36000/mypath`
   * It should returns `http://terrama2_webapp_1:36000/mpmt/mypath` instead.
   *
   * @example
   * console.log(urlResolve('http://terrama2_webapp_1:36000', 'mypath'))
   * http://terrama2_webapp_1:36000/mypath
   *
   * @param {string} from URL origin
   * @param {string} to Path fragment
   * @returns {string} Joined URL
   */
  urlResolve(from, to) {
    const customURL = new url.URL(from);

    customURL.pathname = path.join(customURL.pathname, to);

    return customURL.toString();
  }
}