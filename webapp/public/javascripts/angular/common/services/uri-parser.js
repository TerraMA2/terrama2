define([], function() {
  /**
   * It parses a URI using HTML a tag.
   * 
   * @param {string} uriString - An URI
   * @returns {A}
   */
  function URIParser() {
    var parser = document.createElement('a');

    return function(uriString) {
      parser.href = uriString;
      return parser;
    };
  }

  return URIParser;
});