define([], function() {
    /**
     * It translate title properties of form object
     * 
     * @param {Object} objectToTranslate - Object with properties that contains title
     * @returns {Object} Object with title propertie translated
     */
    function FormTranslator(i18n) {

        return function(objectToTranslate){
            for (var key in objectToTranslate){
                if (objectToTranslate[key].hasOwnProperty('title')){
                    objectToTranslate[key].title = i18n.__(objectToTranslate[key].title);
                }
            }
            return objectToTranslate;
        }
    }

    FormTranslator.$inject = ['i18n'];
    return FormTranslator;
});