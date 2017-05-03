define([], function() {
  /**
   * It translate title properties of form object
   * 
   * @param {Object} objectToTranslate - Object with properties that contains title
   * @returns {Object} Object with title propertie translated
   */
  function FormTranslator(i18n) {
    var isInArray = function(string, array) {
      for(var i = 0, arrayLength = array.length; i < arrayLength; i++) {
        if(array[i] == string) return true;
      }

      return false;
    };

    var translateSchemaFormRequiredError = function(display, required) {
      for(var i = 0, displayLength = display.length; i < displayLength; i++) {
        if(isInArray(display[i].key, required)) {
          display[i].validationMessage = {
            "302": i18n.__("Required")
          };
        }
      }

      return display;
    };

    return function(objectToTranslate, display, required) {
      for(var key in objectToTranslate) {
        if(objectToTranslate[key].hasOwnProperty('title')) {
          objectToTranslate[key].title = i18n.__(objectToTranslate[key].title);
        }
      }

      if(display !== undefined && required !== undefined) {
        return {
          object: objectToTranslate,
          display: translateSchemaFormRequiredError(display, required)
        };
      } else {
        return objectToTranslate;
      }
    };
  }

  FormTranslator.$inject = ['i18n'];
  return FormTranslator;
});