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

    var translateSchemaFormRequiredError = function(display, required, properties) {
      if(display.indexOf('*') != -1) {
        var displayToReturn = [];

        for(var key in properties) {
          var formObject = {
            key: key
          };

          if(properties.hasOwnProperty(key)) {
            if((required !== undefined && isInArray(key, required)) || properties[key].type === "number")
              formObject.validationMessage = {};

            if(required !== undefined && isInArray(key, required)) {
              formObject.validationMessage["302"] = i18n.__("Required");
              formObject.validationMessage["required"] = i18n.__("Required");
            }

            if(properties[key].type === "number") {
              formObject.validationMessage["105"] = i18n.__("Value is not a valid number");
              formObject.validationMessage["number"] = i18n.__("Value is not a valid number");
            }
          }

          displayToReturn.push(formObject);
        }

        return displayToReturn;
      } else {
        for(var i = 0, displayLength = display.length; i < displayLength; i++) {
          if(display[i].validationMessage !== undefined) {
            for(var key in display[i].validationMessage) {
              if(display[i].validationMessage.hasOwnProperty(key)) {
                display[i].validationMessage[key] = i18n.__(display[i].validationMessage[key]);
              }
            }
          }

          if(properties[display[i].key].type === "number") {
            if(display[i].validationMessage === undefined)
              display[i].validationMessage = {};

            display[i].validationMessage["105"] = i18n.__("Value is not a valid number");
            display[i].validationMessage["number"] = i18n.__("Value is not a valid number");
          }

          if(required !== undefined && isInArray(display[i].key, required)) {
            if(display[i].validationMessage === undefined)
              display[i].validationMessage = {};

            display[i].validationMessage["302"] = i18n.__("Required");
            display[i].validationMessage["required"] = i18n.__("Required");
          }
        }

        return display;
      }
    };

    return function(objectToTranslate, display, required) {
      for(var key in objectToTranslate) {
        if(objectToTranslate[key].hasOwnProperty('title')) {
          objectToTranslate[key].title = i18n.__(objectToTranslate[key].title);
        }
      }

      if(display !== undefined) {
        return {
          object: objectToTranslate,
          display: translateSchemaFormRequiredError(display, required, objectToTranslate)
        };
      } else {
        return objectToTranslate;
      }
    };
  }

  FormTranslator.$inject = ['i18n'];
  return FormTranslator;
});