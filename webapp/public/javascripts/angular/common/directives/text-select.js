define(
  function (){
    function terrama2TextSelect(){
      return {
        restrict: 'E',
        template: '<md-autocomplete ' +
                      'required ' +
                      'name="identifier" ' +
                      'md-no-cache="true" ' +
                      'md-selected-item="selectedItem" ' +
                      'md-search-text-change="searchTextChange(searchText)" ' +
                      'md-search-text="searchText" ' +
                      'md-selected-item-change="selectedItemChange(item)" ' +
                      'md-items="item in querySearch(searchText)" ' +
                      'md-item-text="item" ' +
                      'md-min-length="0" ' +
                      'md-dropdown-position="bottom" ' +
                      'md-clear-button="false" ' +
                      'md-input-name="identifier">' +
                    '<md-item-template>' +
                      '<span md-highlight-text="searchText" md-highlight-flags="^i">{{item}}</span>' +
                    '</md-item-template>' +
                  '</md-autocomplete>',
        scope: {
          selectedItem: "=",
          items: "="
        },
        link: function(scope, el, attrs, formCtrl){
          scope.isDisabled = false;

          scope.querySearch = querySearch;
          scope.selectedItemChange = selectedItemChange;
          scope.searchTextChange = searchTextChange;

          // ******************************
          // Internal methods
          // ******************************
          function querySearch (query) {
            var results = query ? scope.items.filter( createFilterFor(query) ) : scope.items;
            return results;
          }

          function searchTextChange(text) {
            scope.selectedItem = text;
          }

          function selectedItemChange(item) {
            if (item){
              // when changed, emit change signal to validate field
              var inputEl = el[0].querySelector("[name]");
              var inputNgEl = angular.element(inputEl);
              inputNgEl.change();
              scope.selectedItem = item;
            }
          }

          /**
           * Create filter function for a query string
           */
          function createFilterFor(query) {
            return function filterFn(state) {
              return (state.indexOf(query) === 0);
            };
          }
        }
      }
    }
    return terrama2TextSelect;
  }
);