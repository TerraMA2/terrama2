define([], function() {
  /**
   * It applies a string format with syntax: {0}, {1}, ...
   * 
   * @param {...string}
   * @returns {string}
   */
  function StringFormat() {
    return function() {
      var theString = arguments[0];
      
      // start with the second argument (i = 1)
      for (var i = 1; i < arguments.length; i++) {
        var regEx = new RegExp("\\{" + (i - 1) + "\\}", "gm");
        theString = theString.replace(regEx, arguments[i]);
      }
      
      return theString;
    };
  }

  return StringFormat;
});