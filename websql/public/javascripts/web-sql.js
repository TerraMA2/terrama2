
$(document).ready(function() {

    var map = new ol.Map({
        target: 'map',
        view: new ol.View({
            extent: [-180, -90, 180, 90],
            center: [-55, -15],
            zoom: 4,
            projection: ol.proj.get("EPSG:4326")
        })
    });

    var addLayer = function(layer){
        map.addLayer(layer);
    };

    addLayer(new ol.layer.Tile({
        source: new ol.source.OSM(),
        id: "osm",
        name: "OpenStreetMap"
    }));

    //event responsible for take the query
    $("#submit").click(function(){
        var data = {
            query: $("#querybox").val()
        };
        $.ajax({
            type: "POST",
            url: "/",
            data: data,
            success: function (msg, status, jqXHR) {
                var jsonUpdatedData = msg;
                console.log(msg);
            }
          });
    });
});