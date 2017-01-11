define([], function() {
  return {
      "GRID": "<?xml version='1.0' encoding='UTF-8'?>" +
                "<StyledLayerDescriptor xmlns='http://www.opengis.net/sld' xmlns:ogc='http://www.opengis.net/ogc' xmlns:xlink='http://www.w3.org/1999/xlink' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.opengis.net/sld http://schemas.opengis.net/sld/1.0.0/StyledLayerDescriptor.xsd' version='1.0.0'>" +
                    "<UserLayer>" +
                      "<Name>{0}-style</Name>" +
                        "<UserStyle>" +
                          "<Name>raster</Name>" +
                          "<FeatureTypeStyle>" +
                          "<FeatureTypeName>Feature</FeatureTypeName>" +
                          "<Rule>" +
                          "<RasterSymbolizer>" +
                          "<Opacity>1.0</Opacity>" +
                          "<ColorMap extended='true'>" +
                            "<ColorMapEntry color='{1}' quantity='{2}'/>" +
                            "<ColorMapEntry color='{3}' quantity='{4}'/>" +
                          "</ColorMap>" +
                        "</RasterSymbolizer>" +
                      "</Rule>" +
                    "</FeatureTypeStyle>" +
                  "</UserStyle>" +
                "</UserLayer>" +
              "</StyledLayerDescriptor>",
      "COMMON": "<?xml version='1.0' encoding='UTF-8'?>" +
                "<StyledLayerDescriptor xmlns='http://www.opengis.net/sld' xmlns:ogc='http://www.opengis.net/ogc' xmlns:xlink='http://www.w3.org/1999/xlink' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' version='1.0.0' xsi:schemaLocation='http://www.opengis.net/sld StyledLayerDescriptor.xsd'>" + 
                  "<NamedLayer>" +
                    "<Name>{0}-style</Name>" +
                    "<UserStyle>" +
                      "<Title>{0}-style</Title>" +
                      "<FeatureTypeStyle>" +
                        // Repeat rule styles
                        "{1}" +
                      "</FeatureTypeStyle>" +
                    "</UserStyle>" +
                  "</NamedLayer>" +
                "</StyledLayerDescriptor>",
    /**
     * It represents the struct for handling Rule based in Geometric Objects (Legend)
     */
    "RULE": "<Rule>" +
              "<se:Name> {1} </se:Name>" +
              "<se:Description>" +
                "<se:Title> {1} </se:Title>" +
              "</se:Description>" +
              "<ogc:Filter xmlns:ogc='http://www.opengis.net/ogc'>" +
                "<ogc:And>" +
                  "<ogc:PropertyIsGreaterThanOrEqualTo>" +
                    "<ogc:PropertyName>{2}</ogc:PropertyName>" +
                    "<ogc:Literal>1</ogc:Literal>" +
                  "</ogc:PropertyIsGreaterThanOrEqualTo>" +
                  "<ogc:PropertyIsLessThanOrEqualTo>" +
                    "<ogc:PropertyName>{2}</ogc:PropertyName>" +
                    "<ogc:Literal>6.20000000000000018</ogc:Literal>" +
                  "</ogc:PropertyIsLessThanOrEqualTo>" +
                "</ogc:And>" +
              "</ogc:Filter>" +
              "<se:PolygonSymbolizer>" +
                "<se:Fill>" +
                  "<se:SvgParameter name='fill'>{3}</se:SvgParameter>" +
                "</se:Fill>" +
                "<se:Stroke>" +
                  "<se:SvgParameter name='stroke'>{3}</se:SvgParameter>" +
                  "<se:SvgParameter name='stroke-width'>0.26000000000000001</se:SvgParameter>" +
                  "<se:SvgParameter name='stroke-linejoin'>bevel</se:SvgParameter>" +
                "</se:Stroke>" +
              "</se:PolygonSymbolizer>" +
            "</Rule>"
    };
});