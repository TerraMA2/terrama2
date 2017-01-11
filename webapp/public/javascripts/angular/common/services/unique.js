define([],
  function() {
    /**
     * It defines 
     */
    function UniqueNumber() {
      function generator() {
        this.length = 5;
        this.timestamp =+ new Date();

        var _getRandomInt = function( min, max ) {
          return Math.floor(Math.random() * ( max - min + 1 )) + min;
        };

        this.generate = function() {
          var ts = this.timestamp.toString();
          var parts = ts.split( "" ).reverse();
          var id = "";

          for( var i = 0; i < this.length; ++i ) {
            var index = _getRandomInt( 0, parts.length - 1 );
            id += parts[index];
          }

          return id;
        };
      }
      return function() {
        var unique = new generator();
        return unique.generate();
      };
    }

    UniqueNumber.$inject = [];

    return UniqueNumber;
  }
)