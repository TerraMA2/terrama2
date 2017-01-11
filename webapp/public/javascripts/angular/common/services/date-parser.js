define([], function() {
  function DateParser() {
    /**
     * It parses a string into a moment date.
     * 
     * @returns {moment.IDate}
     */
    return function(stringDate) {
      return moment.parseZone(stringDate);
    };
  }

  return DateParser;
});