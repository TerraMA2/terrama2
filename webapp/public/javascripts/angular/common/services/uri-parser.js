define([], function() {
  /**
   * It parses a URI using HTML a tag.
   * 
   * @param {string} uri - An URI
   * @returns {A}
   */
  function URIParser() {
    var parser = document.createElement('a');

    return function(uri) {
      if (angular.isObject(uri)) {
        parser.host = uri.host || uri.address;
        parser.port = uri.port;
        parser.pathname = uri.pathname || uri.path;[]
        parser.username = uri.user;
        parser.password = uri.password;
      } else {
        parser.href = uri;
      }

      return parser;
    };
  }

  return URIParser;
});