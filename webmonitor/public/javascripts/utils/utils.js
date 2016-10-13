"use strict";

var getLayersListWithDate = function(mask, layers){
    var firstMaskIndex;
    var lastMaskIndex;
    var firstIndex;
    var lastIndex;
    if (mask.indexOf('yyyy') > 0){
        firstIndex = mask.indexOf('yyyy');
        lastIndex = firstIndex + 3;
        firstMaskIndex = firstIndex;
        lastMaskIndex = lastIndex;
    }
    else {
        firstIndex = mask.indexOf('yy');
        lastIndex = firstIndex + 1;
        firstMaskIndex = firstIndex;
        lastMaskIndex = lastIndex;
    }

    if (mask.indexOf('MM') > 0){
        firstIndex = mask.indexOf('MM');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex){
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    } 

    if (mask.indexOf('dd') > 0){
        firstIndex = mask.indexOf('dd');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex){
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('hh') > 0){
        firstIndex = mask.indexOf('hh');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('mm') > 0){
        firstIndex = mask.indexOf('mm');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('ss') > 0){
        firstIndex = mask.indexOf('ss');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    var dateMask = mask.slice(firstMaskIndex, lastMaskIndex + 1);
    dateMask = dateMask.replace("yyyy", "YYYY");
    dateMask = dateMask.replace("yy", "YY");
    dateMask = dateMask.replace("dd", "DD");

    var layerWithDate = [];
    for (var i=0; i < layers.length; i++){
        var stringLayerDate = layers[i].slice(firstMaskIndex, lastMaskIndex + 1);
        layerWithDate.push({
            layer: layers[i],
            date: moment(stringLayerDate, dateMask)
        })
    }

    function compare(a,b) {
    if (a.date < b.date)
        return -1;
    if (a.date > b.date)
        return 1;
    return 0;
    }

    return layerWithDate.sort(compare);
}
