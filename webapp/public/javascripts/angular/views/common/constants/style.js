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
                        "<Rule>" +
                          "<LineSymbolizer>" +
                            "<Stroke>" +
                              "<CssParameter name='stroke'>{1}</CssParameter>" +
                              "<CssParameter name='stroke-width'>{2}</CssParameter>" +
                            "</Stroke>" +
                          "</LineSymbolizer>" +

                          "<PolygonSymbolizer>" +
                            "<Stroke>" +
                              "<CssParameter name='stroke'>{3}</CssParameter>" +
                              "<CssParameter name='stroke-width'>{4}</CssParameter>" +
                            "</Stroke>" +
                          "</PolygonSymbolizer>" +

                          "<PointSymbolizer>" +
                            "<Graphic>" +
                              "<Mark>" +
                                "<WellKnownName>circle</WellKnownName>" +
                                "<Fill>" +
                                  "<CssParameter name='fill'>{5}</CssParameter>" +
                                "</Fill>" +
                              "</Mark>" +
                              "<Size>{6}</Size>" +
                            "</Graphic>" +
                          "</PointSymbolizer>" +
                        "</Rule>" +
                      "</FeatureTypeStyle>" +
                    "</UserStyle>" +
                  "</NamedLayer>" +
                "</StyledLayerDescriptor>"
    };
});